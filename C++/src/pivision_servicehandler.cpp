#include "pivision_servicehandler.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_services.h"

PiVisionServiceHandler::PiVisionServiceHandler()
{
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SUBSCRIBE_SERVICE_IND, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_CFM, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REJ, this);
}

void PiVisionServiceHandler::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_EVENT_SUBSCRIBE_SERVICE_IND:
    {
      auto subscribeService = std::static_pointer_cast<PiVisionSubscribeServiceInd>(dataPtr);

      auto service = services.find(subscribeService->serviceNo);
      if(services.end() != service)
      {
        if(-1 != service->second)
        {
          services[subscribeService->serviceNo] = -1;
          auto serviceAvail = std::make_shared<PiVisionServiceAvailableInd>(service->first);
          JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_AVAILABLE_IND, serviceAvail);
        }
      }
      else
      {
        auto connectService = std::make_shared<PiVisionConnectToServiceReq>(subscribeService->serviceNo);
        JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REQ, connectService);
      }
    }
    break;
    case PIVISION_EVENT_CONNECT_TO_SERVICE_CFM:
    {
      auto connectCfm = std::static_pointer_cast<PiVisionConnectToServiceCfm>(dataPtr);
      auto service = services.find(connectCfm->serviceNo);

      if(-1 == service->second)
      {
        services[connectCfm->serviceNo] = connectCfm->serviceId;
        auto serviceAvail = std::make_shared<PiVisionServiceAvailableInd>(connectCfm->serviceNo);
        JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_AVAILABLE_IND, serviceAvail);
      }
    }
    break;
    case PIVISION_EVENT_CONNECT_TO_SERVICE_REJ:
    {
      auto connectRej = std::static_pointer_cast<PiVisionConnectToServiceRej>(dataPtr);
      if(PIVISION_CAMERA_SERVICE == connectRej->serviceNo)
      {
        JobDispatcher::GetApi()->Log("ServiceHandler could not locate camera. Abort.");
        JobDispatcher::GetApi()->RaiseEventIn(PIVISION_EVENT_STOP, nullptr, 500u);
      }
    }
    break;
    default:
      JobDispatcher::GetApi()->Log("ServiceHandler received unexpected event: 0x%x", eventNo);
      break;
  }
}
