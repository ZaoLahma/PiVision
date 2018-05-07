#ifndef HEADER_PIVISION_CLIENT
#define HEADER_PIVISION_CLIENT

#include "eventlistenerbase.h"

class PiVisionClient : public EventListenerBase
{
private:
  const uint32_t serviceNo;

protected:

public:
  PiVisionClient(const uint32_t _serviceNo);
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);

};

#endif
