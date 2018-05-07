#include "pivision_framecoord.h"
#include "jobdispatcher.h"
#include "pivision_events.h"

PiVisionFrameCoord::PiVisionFrameCoord() : currFrame(0u), running(true)
{
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_STOP, this);
}

void PiVisionFrameCoord::Execute()
{
  JobDispatcher::GetApi()->Log("PiVisionFrameCoord started");

  while(running)
  {
    /* Do clever stuff here? */
  }
}

void PiVisionFrameCoord::HandleEvent(const uint32_t eventNo,
                                     std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_EVENT_STOP:
      running = false;
      JobDispatcher::GetApi()->NotifyExecutionFinished();
      break;
    default:
      JobDispatcher::GetApi()->Log("PiVisionFrameCoord received unexpected event: 0x%x", eventNo);
      break;
  }
}
