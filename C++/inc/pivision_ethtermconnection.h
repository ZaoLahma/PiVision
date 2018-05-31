#ifndef HEADER_PIVISION_ETHTERM_CONNECTION
#define HEADER_PIVISION_ETHTERM_CONNECTION

#include "jobbase.h"
#include "eventlistenerbase.h"
#include "pivision_events.h"

#include <mutex>

class PiVisionEthTermConnection : public JobBase, public EventListenerBase
{
private:
  void Receive(const uint32_t numBytesToGet, std::shared_ptr<PiVisionDataBuf> dataBuf);
  void Send(const std::shared_ptr<PiVisionDataBuf> dataBuf);
  bool active;
  const PiVisionConnectionType connType;
  const uint32_t serviceNo;
  const int32_t socketFd;
  std::mutex sendMutex;

protected:

public:
  PiVisionEthTermConnection(const PiVisionConnectionType _connType,
                            const uint32_t _serviceNo,
                            const int32_t _socketFd);
  ~PiVisionEthTermConnection();
  void Execute();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

#endif
