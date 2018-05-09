#ifndef HEADER_PIVISION_ETHTERM
#define HEADER_PIVISION_ETHTERM

#include "jobbase.h"
#include "eventlistenerbase.h"

class PiVisionEthTerm : public EventListenerBase
{
private:

protected:

public:
  PiVisionEthTerm();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

#endif
