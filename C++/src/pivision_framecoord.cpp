#include "pivision_framecoord.h"
#include "jobdispatcher.h"
#include "pivision_events.h"

PiVisionFrameCoord::PiVisionFrameCoord() : currFrame(0u), running(true)
{
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_STOP, this);
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_NEW_FRAME_IND, this);
}

void PiVisionFrameCoord::Execute()
{
  JobDispatcher::GetApi()->Log("PiVisionFrameCoord started");

  while(running)
  {
    /* Do clever stuff here? */
  }

  JobDispatcher::GetApi()->NotifyExecutionFinished();
}

void PiVisionFrameCoord::HandleEvent(const uint32_t eventNo,
                                     std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_EVENT_NEW_FRAME_IND:
    {
      auto newFrameInd = std::static_pointer_cast<PiVisionNewFrameInd>(dataPtr);
      currFrame += 1u;
      JobDispatcher::GetApi()->Log("New frame: %u", currFrame);
      break;
    }
    case PIVISION_EVENT_STOP:
      running = false;
      break;
    default:
      JobDispatcher::GetApi()->Log("PiVisionFrameCoord received unexpected event: 0x%x", eventNo);
      break;
  }
}
