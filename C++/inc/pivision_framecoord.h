#ifndef HEADER_PIVISION_FRAMECOORD
#define HEADER_PIVISION_FRAMECOORD

#include <cstddef>

#include "jobbase.h"
#include "eventlistenerbase.h"

class PiVisionFrameCoord : public JobBase, public EventListenerBase
{
private:
  uint32_t currFrame;
  bool running;
protected:

public:
  PiVisionFrameCoord();
  void Execute();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

#endif
