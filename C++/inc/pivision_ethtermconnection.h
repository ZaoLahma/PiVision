#ifndef HEADER_PIVISION_ETHTERM_CONNECTION
#define HEADER_PIVISION_ETHTERM_CONNECTION

#include "jobbase.h"
#include "eventlistenerbase.h"
#include "pivision_events.h"

class PiVisionEthTermConnection : public JobBase, public EventListenerBase
{
private:
  void Receive(const uint32_t numBytesToGet, PiVisionDataBuf& dataBuf);
  bool active;
  const uint32_t serviceNo;
  const int32_t socketFd;

protected:

public:
  PiVisionEthTermConnection(const uint32_t _serviceNo, const int32_t _socketFd);
  ~PiVisionEthTermConnection();
  void Execute();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

#endif
