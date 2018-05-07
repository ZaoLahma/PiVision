#include "pivision_client.h"
#include "jobdispatcher.h"
#include "pivision_events.h"

PiVisionClient::PiVisionClient(const uint32_t _serviceNo) : serviceNo(_serviceNo)
{
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_CFM, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REJ, this);

  std::shared_ptr<EventDataBase> serviceReq = std::make_shared<PiVisionConnectToServiceReq>(serviceNo);
  JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REQ, serviceReq);
}

void PiVisionClient::HandleEvent(const uint32_t eventNo,
                                 std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_EVENT_CONNECT_TO_SERVICE_CFM:
      JobDispatcher::GetApi()->Log("PivisionClient connected to service %u", serviceNo);
      //JobDispatcher::GetApi()->RaiseEventIn(PIVISION_EVENT_STOP, nullptr, 100u);
      break;
    case PIVISION_EVENT_CONNECT_TO_SERVICE_REJ:
      JobDispatcher::GetApi()->Log("Received ConnectToServiceRej. Execution aborted.");
      JobDispatcher::GetApi()->RaiseEventIn(PIVISION_EVENT_STOP, nullptr, 100u);
      break;
    default:
      JobDispatcher::GetApi()->Log("PivisionClient received unexpected event: 0x%x", eventNo);
      break;
  }
}
