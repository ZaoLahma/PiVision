#include "pivision_ethterm.h"
#include "jobdispatcher.h"
#include "pivision_threadmodel.h"
#include "pivision_ethtermconnectservice.h"
#include "pivision_ethtermconnection.h"
#include "pivision_ethtermservicelistener.h"
#include "pivision_events.h"

PiVisionEthTerm::PiVisionEthTerm()
{
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REQ, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SERVICE_STATUS_IND, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_PROVIDE_SERVICE_IND, this);
}

void PiVisionEthTerm::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_EVENT_PROVIDE_SERVICE_IND:
    {
      auto provideInd = std::static_pointer_cast<PiVisionProvideServiceInd>(dataPtr);
      auto serviceListener = std::make_shared<PiVisionEthTermServiceListener>(provideInd->serviceNo);
      JobDispatcher::GetApi()->ExecuteJobInGroup(serviceListener, PIVISION_SERVICE_LISTENER_THREAD_ID);
    }
    break;
    case PIVISION_EVENT_CONNECT_TO_SERVICE_REQ:
    {
      auto serviceReq = std::static_pointer_cast<PiVisionConnectToServiceReq>(dataPtr);
      auto serviceReqJob = std::make_shared<PiVisionEthTermConnectService>(serviceReq->serviceNo);
      JobDispatcher::GetApi()->ExecuteJobInGroup(serviceReqJob, PIVISION_SERVICE_DISCOVERY_THREAD_ID);
    }
    break;
    case PIVISION_EVENT_SERVICE_STATUS_IND:
    {
      std::shared_ptr<PiVisionConnectionStatusInd> statusInd = std::static_pointer_cast<PiVisionConnectionStatusInd>(dataPtr);
      switch(statusInd->status)
      {
        case PiVisionServiceStatus::SERVICE_DISCONNECTED:
        case PiVisionServiceStatus::SERVICE_NOT_FOUND:
        {
          std::shared_ptr<EventDataBase> connectRej = std::make_shared<PiVisionConnectToServiceRej>(statusInd->serviceNo);
          JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REJ, connectRej);
        }
        break;
        case PiVisionServiceStatus::SERVICE_CONNECTED:
        {
          auto connectCfm = std::make_shared<PiVisionConnectToServiceCfm>(statusInd->serviceNo,
                                                                          (uint32_t)statusInd->socketFd);

          JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_CFM, connectCfm);

          auto connectionJob = std::make_shared<PiVisionEthTermConnection>(statusInd->serviceNo, statusInd->socketFd);
          JobDispatcher::GetApi()->ExecuteJobInGroup(connectionJob, PIVISION_CONNECTIONS_THREAD_ID);
        }
        break;
        default:
        break;
      }
      break;
    }
    default:
      JobDispatcher::GetApi()->Log("PiVisionEthTerm received unexpected event: 0x%x", eventNo);
      break;
  }
}
