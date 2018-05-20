#ifndef HEADER_PIVISION_GRAYSCALEIMAGE
#define HEADER_PIVISION_GRAYSCALEIMAGE

#include "eventlistenerbase.h"

class PiVisionGrayscaleImage : public EventListenerBase
{
private:

protected:

public:
  PiVisionGrayscaleImage();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

#endif
