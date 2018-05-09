#include "pivision_framecoord.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_services.h"

PiVisionFrameCoord::PiVisionFrameCoord() :
currFrame(0u),
cameraServiceAvailable(false)
{
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SERVICE_AVAILABLE_IND, this);
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SERVICE_UNAVAILABLE_IND, this);
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_STOP, this);
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_CAMERA_SERVICE, this);

    auto subscribeService = std::make_shared<PiVisionSubscribeServiceInd>(PIVISION_CAMERA_SERVICE);
    JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SUBSCRIBE_SERVICE_IND, subscribeService);
}

void PiVisionFrameCoord::HandleEvent(const uint32_t eventNo,
                                     std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_EVENT_SERVICE_UNAVAILABLE_IND:
    {
      auto serviceUnavailable = std::static_pointer_cast<PiVisionServiceUnavailableInd>(dataPtr);
      if(PIVISION_CAMERA_SERVICE == serviceUnavailable->serviceNo)
      {
        JobDispatcher::GetApi()->Log("PiVisionFrameCoord lost camera. Abort.");
        JobDispatcher::GetApi()->RaiseEventIn(PIVISION_EVENT_STOP, nullptr, 500u);
      }
    }
    break;
    case PIVISION_EVENT_SERVICE_AVAILABLE_IND:
    {
      auto service = std::static_pointer_cast<PiVisionServiceAvailableInd>(dataPtr);
      if(PIVISION_CAMERA_SERVICE == service->serviceNo)
      {
        JobDispatcher::GetApi()->Log("PiVisionFrameCoord connected to camera");
        cameraServiceAvailable = true;
      }
    }
    break;
    case PIVISION_CAMERA_SERVICE:
    {
      auto newDataInd = std::static_pointer_cast<PiVisionNewDataInd>(dataPtr);
      currFrame += 1u;
      JobDispatcher::GetApi()->Log("New frame: %u", currFrame);
      break;
    }
    case PIVISION_EVENT_STOP:
      JobDispatcher::GetApi()->NotifyExecutionFinished();
      break;
    default:
      JobDispatcher::GetApi()->Log("PiVisionFrameCoord received unexpected event: 0x%x", eventNo);
      break;
  }
}
