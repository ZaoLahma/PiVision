#include "pivision_framecoord.h"
#include "jobdispatcher.h"
#include "pivision_events.h"

PiVisionFrameCoord::PiVisionFrameCoord() : currFrame(0u), running(true)
{
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_STOP, this);
}

void PiVisionFrameCoord::Execute()
{
  JobDispatcher::GetApi()->Log("PiVisionFrameCoord started");

  while(running)
  {

  }
}

void PiVisionFrameCoord::HandleEvent(const uint32_t eventNo,
                                     std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_STOP:
      running = false;
      break;
    default:
      JobDispatcher::GetApi()->Log("PiVisionFrameCoord received unexpected event: 0x%x", eventNo);
      break;
  }
}
