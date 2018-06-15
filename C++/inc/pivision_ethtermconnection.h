#ifndef HEADER_PIVISION_ETHTERM_CONNECTION
#define HEADER_PIVISION_ETHTERM_CONNECTION

#include "jobbase.h"
#include "eventlistenerbase.h"
#include "pivision_events.h"

#include <condition_variable>
#include <mutex>

class PiVisionEthTermConnectionHBTimeout : public EventDataBase
{
private:
  PiVisionEthTermConnectionHBTimeout();

protected:

public:
  const int32_t id;
  PiVisionEthTermConnectionHBTimeout(const int32_t _id) : id(_id)
  {

  }
};

class PiVisionEthTermConnection : public JobBase, public EventListenerBase
{
private:
  void Receive(const uint32_t numBytesToGet, std::shared_ptr<PiVisionDataBuf> dataBuf);
  void SendHeader(const std::shared_ptr<PiVisionDataBuf> dataBuf);
  void SendPayload(const std::shared_ptr<PiVisionDataBuf> dataBuf);
  void Send(const std::shared_ptr<PiVisionDataBuf> dataBuf);
  bool active;
  const PiVisionConnectionType connType;
  const uint32_t serviceNo;
  const int32_t socketFd;
  std::mutex sendMutex;
  std::condition_variable lastExecNotification;
  const uint32_t ackMsg;
  const uint32_t heartbeatMsg;
  const uint32_t HEARTBEAT_TIMEOUT;
  const uint32_t HEARTBEAT_PERIODICITY;
  bool receivedAck;
  bool ackEnabled;
  std::shared_ptr<PiVisionDataBuf> ackMsgBuf;
  std::shared_ptr<PiVisionDataBuf> heartbeatMsgBuf;

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
