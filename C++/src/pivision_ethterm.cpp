#include "pivision_ethterm.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_threadmodel.h"

PiVisionEthTermConnectServiceJob::PiVisionEthTermConnectServiceJob(const uint32_t _serviceNo):
serviceNo(_serviceNo),
serviceDiscoveryHeader("WHERE_IS_")
{

}

void PiVisionEthTermConnectServiceJob::Execute()
{
  std::string serviceDiscoverString = serviceDiscoveryHeader + std::to_string(serviceNo);
  JobDispatcher::GetApi()->Log("serviceDiscoveryString: %s", serviceDiscoverString.c_str());
  std::shared_ptr<EventDataBase> serviceRej = std::make_shared<PiVisionConnectToServiceRej>(serviceNo);
  JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REJ, serviceRej);
}

PiVisionEthTerm::PiVisionEthTerm()
{
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REQ, this);
}

void PiVisionEthTerm::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_EVENT_CONNECT_TO_SERVICE_REQ:
    {
      std::shared_ptr<PiVisionConnectToServiceReq> serviceReq = std::static_pointer_cast<PiVisionConnectToServiceReq>(dataPtr);
      std::shared_ptr<JobBase> serviceReqJob = std::make_shared<PiVisionEthTermConnectServiceJob>(serviceReq->serviceNo);
      JobDispatcher::GetApi()->ExecuteJobInGroup(serviceReqJob, PIVISION_SERVICE_DISCOVERY_THREAD_ID);
      break;
    }
    default:
      JobDispatcher::GetApi()->Log("PiVisionEthTerm received unexpected event: 0x%x", eventNo);
      break;
  }
}
