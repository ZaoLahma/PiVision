#ifndef HEADER_PIVISION_ETHTERM
#define HEADER_PIVISION_ETHTERM

#include <string>
#include <thread>

#include "jobbase.h"
#include "eventlistenerbase.h"

class PiVisionEthTermConnectServiceJob : public JobBase, public EventListenerBase
{
private:
  PiVisionEthTermConnectServiceJob();
  void TryConnect();
  int32_t ConnectToServer();
  const uint32_t serviceNo;
  const std::string serviceDiscoveryHeader;
  const std::string multicastGroup;
  uint32_t numAttempts;
  int32_t serviceDiscoverySocket;
  std::mutex timeoutMutex;
	std::condition_variable timeoutNotification;
  std::string serverAddress;
  bool serviceFound;
  int32_t socketFd;

protected:

public:
  PiVisionEthTermConnectServiceJob(const uint32_t _serviceNo);
  void Execute();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

class PiVisionEthTerm : public EventListenerBase
{
private:

protected:

public:
  PiVisionEthTerm();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

#endif
