#ifndef HEADER_PIVISION_ETHTERM_CONNECT_SERVICE
#define HEADER_PIVISION_ETHTERM_CONNECT_SERVICE

#include <string>
#include <mutex>
#include <condition_variable>

#include "jobbase.h"
#include "eventlistenerbase.h"

class PiVisionEthTermConnectService : public JobBase, public EventListenerBase
{
private:
  PiVisionEthTermConnectService();
  void FindService();
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
  PiVisionEthTermConnectService(const uint32_t _serviceNo);
  void Execute();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

#endif
