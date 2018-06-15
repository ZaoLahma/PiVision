#ifndef HEADER_PIVISION_GRAYSCALEIMAGE
#define HEADER_PIVISION_GRAYSCALEIMAGE

#include "eventlistenerbase.h"
#include <cstddef>

class PiVisionGrayscaleImage : public EventListenerBase
{
private:
  uint32_t numJobsOngoing;
  const uint32_t NUM_JOBS_LIMIT;

protected:

public:
  PiVisionGrayscaleImage();
  ~PiVisionGrayscaleImage();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

#endif
