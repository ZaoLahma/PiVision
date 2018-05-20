#include "pivision_grayscaleimage.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_services.h"

PiVisionGrayscaleImage::PiVisionGrayscaleImage()
{
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SERVICE_AVAILABLE_IND, this);

  auto subscribeService = std::make_shared<PiVisionSubscribeServiceInd>(PIVISION_COLOR_IMAGE_SERVICE);
  JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SUBSCRIBE_SERVICE_IND, subscribeService);
}

void PiVisionGrayscaleImage::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_COLOR_IMAGE_SERVICE:
    {
      /*
        TODO:
        publish grayscale image service
        convert color image to gracyscale
      */
      JobDispatcher::GetApi()->Log("PiVisionGrayscaleImage received color image");
    }
    break;
    case PIVISION_EVENT_SERVICE_AVAILABLE_IND:
    {
      auto serviceAvailable = std::static_pointer_cast<PiVisionServiceAvailableInd>(dataPtr);
      if(PIVISION_COLOR_IMAGE_SERVICE == serviceAvailable->serviceNo)
      {
        JobDispatcher::GetApi()->Log("PiVisionGrayscaleImage found color image service");
        JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_COLOR_IMAGE_SERVICE, this);
      }
    }
    break;
    default:
      JobDispatcher::GetApi()->Log("PiVisionGrayscaleImage received unexpected event: 0x%x", eventNo);
      break;
  }
}
