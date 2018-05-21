#ifndef HEADER_PIVISION_SERVICELISTENER
#define HEADER_PIVISION_SERVICELISTENER

#include "jobbase.h"
#include "eventlistenerbase.h"
#include <string>
#include <arpa/inet.h>

class PiVisionEthTermServiceListener : public JobBase, public EventListenerBase
{
private:
  int initiateServiceDiscoverySocket();

  const uint32_t serviceNo;
  bool active;
  int32_t serviceDiscoverySocket;
  struct sockaddr_in addr;

protected:

public:
  PiVisionEthTermServiceListener(const uint32_t _serviceNo);
  ~PiVisionEthTermServiceListener();
  void Execute();
  void HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr);
};

#endif
