#include "pivision_grayscaleimage.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_services.h"
#include "pivision_grayscaleimagejob.h"

PiVisionGrayscaleImage::PiVisionGrayscaleImage()
{
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SERVICE_AVAILABLE_IND, this);

  auto subscribeService = std::make_shared<PiVisionSubscribeServiceInd>(PIVISION_COLOR_IMAGE_SERVICE_RX);
  JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SUBSCRIBE_SERVICE_IND, subscribeService);
}

void PiVisionGrayscaleImage::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_COLOR_IMAGE_SERVICE_RX:
    {
      auto imageData = std::static_pointer_cast<PiVisionNewDataInd>(dataPtr);

      auto grayscaleJob = std::make_shared<PiVisionGrayscaleImageJob>(imageData);
      JobDispatcher::GetApi()->ExecuteJob(grayscaleJob);

      auto ackMessage = std::make_shared<PiVisionNewDataInd>(PiVisionConnectionType::PIVISION_CLIENT, std::make_shared<PiVisionDataBuf>());
      JobDispatcher::GetApi()->RaiseEvent(PIVISION_COLOR_IMAGE_SERVICE_TX, ackMessage);
    }
    break;
    case PIVISION_EVENT_SERVICE_AVAILABLE_IND:
    {
      auto serviceAvailable = std::static_pointer_cast<PiVisionServiceAvailableInd>(dataPtr);
      if(PIVISION_COLOR_IMAGE_SERVICE_RX == serviceAvailable->serviceNo)
      {
        JobDispatcher::GetApi()->Log("PiVisionGrayscaleImage found color image service");
        JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_COLOR_IMAGE_SERVICE_RX, this);
        auto serviceProvided = std::make_shared<PiVisionServiceAvailableInd>(PIVISION_BW_IMAGE_SERVICE_RX);
        JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_PROVIDED_IND, serviceProvided);
      }
    }
    break;
    default:
      JobDispatcher::GetApi()->Log("PiVisionGrayscaleImage received unexpected event: 0x%x", eventNo);
      break;
  }
}
