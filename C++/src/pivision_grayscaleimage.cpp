#include "pivision_grayscaleimage.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_services.h"
#include "pivision_grayscaleimagejob.h"
#include "pivision_threadmodel.h"

PiVisionGrayscaleImage::PiVisionGrayscaleImage() :
numJobsOngoing(0u),
NUM_JOBS_LIMIT(1u)
{
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SERVICE_AVAILABLE_IND, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_GRAYSCALE_JOB_COMPLETE_IND, this);

  auto subscribeService = std::make_shared<PiVisionSubscribeServiceInd>(PIVISION_COLOR_IMAGE_SERVICE_RX);
  JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SUBSCRIBE_SERVICE_IND, subscribeService);
}

PiVisionGrayscaleImage::~PiVisionGrayscaleImage()
{
  JobDispatcher::GetApi()->UnsubscribeToEvent(PIVISION_EVENT_SERVICE_AVAILABLE_IND, this);
  JobDispatcher::GetApi()->UnsubscribeToEvent(PIVISION_EVENT_GRAYSCALE_JOB_COMPLETE_IND, this);
}

void PiVisionGrayscaleImage::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_COLOR_IMAGE_SERVICE_RX:
    {
      if(NUM_JOBS_LIMIT > numJobsOngoing)
      {
        auto imageData = std::static_pointer_cast<PiVisionNewDataInd>(dataPtr);
        auto grayscaleJob = std::make_shared<PiVisionGrayscaleImageJob>(imageData);
        JobDispatcher::GetApi()->ExecuteJobInGroup(grayscaleJob, PIVISION_GRAYSCALE_JOB_THREAD_ID);
        numJobsOngoing += 1u;
      }
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
    case PIVISION_EVENT_GRAYSCALE_JOB_COMPLETE_IND:
    {
      numJobsOngoing -= 1u;
    }
    break;
    default:
      JobDispatcher::GetApi()->Log("PiVisionGrayscaleImage received unexpected event: 0x%x", eventNo);
      break;
  }
}
