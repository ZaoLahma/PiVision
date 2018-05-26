#include "pivision_servicehandler.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_services.h"

PiVisionServiceHandler::PiVisionServiceHandler() :
LOCAL_SERVICE(0),
REMOTE_SERVICE_DISCONNECTED(-1)
{
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SUBSCRIBE_SERVICE_IND, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_CFM, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REJ, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SERVICE_PROVIDED_IND, this);
}

void PiVisionServiceHandler::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_EVENT_SERVICE_PROVIDED_IND:
    {
      auto newService = std::static_pointer_cast<PiVisionServiceAvailableInd>(dataPtr);
      auto service = services.find(newService->serviceNo);

      if(services.end() == service ||
         ((services.end() != service) &&
         (REMOTE_SERVICE_DISCONNECTED == service->second)))
      {
        services[newService->serviceNo] = LOCAL_SERVICE;
        auto serviceAvail = std::make_shared<PiVisionServiceAvailableInd>(newService->serviceNo);
        JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_AVAILABLE_IND, serviceAvail);

        auto provideService = std::make_shared<PiVisionProvideServiceInd>(newService->serviceNo);
        JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_PROVIDE_SERVICE_IND, provideService);
      }
    }
    break;
    case PIVISION_EVENT_SUBSCRIBE_SERVICE_IND:
    {
      auto subscribeService = std::static_pointer_cast<PiVisionSubscribeServiceInd>(dataPtr);

      auto service = services.find(subscribeService->serviceNo);
      if(services.end() != service)
      {
        if(REMOTE_SERVICE_DISCONNECTED != service->second)
        {
          /* Service has been published by local or network actor */
          auto serviceAvail = std::make_shared<PiVisionServiceAvailableInd>(service->first);
          JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_AVAILABLE_IND, serviceAvail);
        }
      }
      else
      {
        /* If service has not been published by local actor, ask network */
        services[subscribeService->serviceNo] = REMOTE_SERVICE_DISCONNECTED;
        auto connectService = std::make_shared<PiVisionConnectToServiceReq>(subscribeService->serviceNo);
        JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REQ, connectService);
      }
    }
    break;
    case PIVISION_EVENT_CONNECT_TO_SERVICE_CFM:
    {
      auto connectCfm = std::static_pointer_cast<PiVisionConnectToServiceCfm>(dataPtr);
      auto service = services.find(connectCfm->serviceNo);

      if(REMOTE_SERVICE_DISCONNECTED == service->second)
      {
        JobDispatcher::GetApi()->Log("Received ConnectToServiceCfm for service %u. Starting connection.", connectCfm->serviceNo);
        services[connectCfm->serviceNo] = connectCfm->serviceId;
        auto serviceAvail = std::make_shared<PiVisionServiceAvailableInd>(connectCfm->serviceNo);
        JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_AVAILABLE_IND, serviceAvail);
      }
    }
    break;
    case PIVISION_EVENT_CONNECT_TO_SERVICE_REJ:
    {
      auto connectRej = std::static_pointer_cast<PiVisionConnectToServiceRej>(dataPtr);
      auto serviceUnavailable = std::make_shared<PiVisionServiceUnavailableInd>(connectRej->serviceNo);
      JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_UNAVAILABLE_IND, serviceUnavailable);
    }
    break;
    default:
      JobDispatcher::GetApi()->Log("ServiceHandler received unexpected event: 0x%x", eventNo);
      break;
  }
}
