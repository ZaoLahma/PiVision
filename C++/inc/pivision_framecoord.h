#ifndef HEADER_PIVISION_FRAMECOORD
#define HEADER_PIVISION_FRAMECOORD

#include <cstddef>

#include "eventlistenerbase.h"

class PiVisionFrameCoord : public EventListenerBase
{
private:
  uint32_t currFrame;
  bool cameraServiceAvailable;
protected:

public:
  PiVisionFrameCoord();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

#endif
