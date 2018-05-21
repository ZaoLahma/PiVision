#include "pivision_ethtermservicelistener.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_macros.h"

PiVisionEthTermServiceListener::PiVisionEthTermServiceListener(const uint32_t _serviceNo) :
serviceNo(_serviceNo),
active(true)
{
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_STOP, this);
}

PiVisionEthTermServiceListener::~PiVisionEthTermServiceListener()
{
  JobDispatcher::GetApi()->UnsubscribeToEvent(PIVISION_EVENT_STOP, this);
}

void PiVisionEthTermServiceListener::Execute()
{
  JobDispatcher::GetApi()->Log("Service %u published to network", serviceNo);
  while(active)
  {
    //...
  }
}

void PiVisionEthTermServiceListener::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  PIVISION_UNUSED_ARG(dataPtr);

  switch(eventNo)
  {
    case PIVISION_EVENT_STOP:
    active = false;
    break;
    default:
    JobDispatcher::GetApi()->Log("PiVisionEthTermServiceListener received unexpected event: 0x%X", eventNo);
    break;
  }
}
