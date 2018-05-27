#include "pivision_framecoord.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_services.h"
#include <iostream>

PiVisionFrameCoord::PiVisionFrameCoord() :
currFrame(0u),
cameraServiceAvailable(false)
{
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SERVICE_AVAILABLE_IND, this);
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SERVICE_UNAVAILABLE_IND, this);
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_STOP, this);
    JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_CAMERA_SERVICE_RX, this);

    auto subscribeService = std::make_shared<PiVisionSubscribeServiceInd>(PIVISION_CAMERA_SERVICE_RX);
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
      if(PIVISION_CAMERA_SERVICE_RX == serviceUnavailable->serviceNo)
      {
        JobDispatcher::GetApi()->Log("PiVisionFrameCoord lost camera. Abort.");
        JobDispatcher::GetApi()->RaiseEventIn(PIVISION_EVENT_STOP, nullptr, 500u);
      }
    }
    break;
    case PIVISION_EVENT_SERVICE_AVAILABLE_IND:
    {
      auto service = std::static_pointer_cast<PiVisionServiceAvailableInd>(dataPtr);
      if(PIVISION_CAMERA_SERVICE_RX == service->serviceNo)
      {
        JobDispatcher::GetApi()->Log("PiVisionFrameCoord connected to camera");
        cameraServiceAvailable = true;

        auto serviceProvided = std::make_shared<PiVisionServiceAvailableInd>(PIVISION_COLOR_IMAGE_SERVICE_RX);
        JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_PROVIDED_IND, serviceProvided);
      }
    }
    break;
    case PIVISION_CAMERA_SERVICE_RX:
    {
      auto newDataInd = std::static_pointer_cast<PiVisionNewDataInd>(dataPtr);
      currFrame += 1u;
      JobDispatcher::GetApi()->RaiseEvent(PIVISION_COLOR_IMAGE_SERVICE_TX, newDataInd);
    }
    break;
    case PIVISION_EVENT_STOP:
      JobDispatcher::GetApi()->NotifyExecutionFinished();
      break;
    default:
      JobDispatcher::GetApi()->Log("PiVisionFrameCoord received unexpected event: 0x%x", eventNo);
      break;
  }
}
