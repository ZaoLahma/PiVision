#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "pivision_ethtermconnectservice.h"
#include "jobdispatcher.h"
#include "pivision_events.h"

PiVisionEthTermConnectService::PiVisionEthTermConnectService(const uint32_t _serviceNo):
serviceNo(_serviceNo),
serviceDiscoveryHeader("WHERE_IS_"),
multicastGroup("224.1.1.1"),
numAttempts(0u),
serverAddress(""),
serviceFound(false),
socketFd(0)
{
  serviceDiscoverySocket = socket(AF_INET, SOCK_DGRAM, 0);

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(serviceDiscoverySocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SERVICE_DISCOVERY_TIMEOUT, this);
}

PiVisionEthTermConnectService::~PiVisionEthTermConnectService()
{
  JobDispatcher::GetApi()->UnsubscribeToEvent(PIVISION_EVENT_SERVICE_DISCOVERY_TIMEOUT, this);
}

void PiVisionEthTermConnectService::FindService(void)
{
  std::string serviceDiscoverString = serviceDiscoveryHeader + std::to_string(serviceNo);
  JobDispatcher::GetApi()->Log("serviceDiscoveryString: %s", serviceDiscoverString.c_str());

  struct sockaddr_in addr;

  memset(&addr,0,sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(multicastGroup.c_str());
  addr.sin_port = htons(serviceNo);

  int numBytesSent = sendto(serviceDiscoverySocket,
                            serviceDiscoverString.c_str(),
                            serviceDiscoverString.length(),
                            0,
                            (struct sockaddr*)&addr,
                            sizeof(addr));
  if(numBytesSent < 0)
  {
    perror("sendto");
    exit(1);
  }

  char serverAddressArray[INET_ADDRSTRLEN];

  int32_t bytesReceived = recv(serviceDiscoverySocket,
                               serverAddressArray,
                               sizeof(serverAddressArray),
                               0);

  if(bytesReceived > 0)
  {
    serverAddressArray[bytesReceived] = '\0';
    serverAddress = std::string(serverAddressArray);
    JobDispatcher::GetApi()->Log("PiVisionEthTermConnectService Received service address: %s",
                                 serverAddress.c_str());
    serviceFound = true;
  }
}

int32_t PiVisionEthTermConnectService::ConnectToServer()
{
  int sockfd;

  struct addrinfo hints;
  struct addrinfo* servinfo;
  struct addrinfo* p;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(serverAddress.c_str(),
                  std::to_string(serviceNo).c_str(),
                  &hints,
                  &servinfo) != 0)
  {
      return -1;
  }

  for(p = servinfo; p != 0; p = p->ai_next)
  {
      if ((sockfd = socket(p->ai_family, p->ai_socktype,
                           p->ai_protocol)) == -1)
      {
          continue;
      }

      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
      {
          close(sockfd);
          continue;
      }

      break;
  }

  if (p == 0)
  {
      printf("p==0, errno: %d\n", errno);
      return -1;
  }

  freeaddrinfo(servinfo);

  return sockfd;
}

void PiVisionEthTermConnectService::Execute()
{
  const uint32_t maxNumAttempts = 5u;
  serviceFound = false;
  while(numAttempts < maxNumAttempts)
  {
    FindService();
    if(!serviceFound)
    {
      numAttempts += 1u;

      auto serviceTimeout = std::make_shared<PiVisionServiceDiscoveryTimeout>(serviceNo);
      JobDispatcher::GetApi()->RaiseEventIn(PIVISION_EVENT_SERVICE_DISCOVERY_TIMEOUT, serviceTimeout, 500u);

      std::unique_lock<std::mutex> timeoutLock(timeoutMutex);
      timeoutNotification.wait(timeoutLock);
    }
    else
    {
      numAttempts = maxNumAttempts;
    }
  }

  std::shared_ptr<PiVisionConnectionStatusInd> connectionStatusIndInd = nullptr;

  if(serviceFound)
  {
    JobDispatcher::GetApi()->Log("PiVisionEthTermConnectService Service %u found", serviceNo);
    if(-1 != (socketFd = ConnectToServer()))
    {
      PiVisionServiceStatus status = PiVisionServiceStatus::SERVICE_CONNECTED;
      connectionStatusIndInd = std::make_shared<PiVisionConnectionStatusInd>(status, serviceNo, socketFd);
      JobDispatcher::GetApi()->Log("PiVisionEthTermConnectService PiVision connected to service %u", serviceNo);
    }
    else
    {
      JobDispatcher::GetApi()->Log("PiVisionEthTermConnectService Failed to connect to service %u", serviceNo);
      PiVisionServiceStatus status = PiVisionServiceStatus::SERVICE_DISCONNECTED;
      connectionStatusIndInd = std::make_shared<PiVisionConnectionStatusInd>(status, serviceNo, -1);
    }
  }
  else
  {
    JobDispatcher::GetApi()->Log("PiVisionEthTermConnectService Service %u not found", serviceNo);
    PiVisionServiceStatus status = PiVisionServiceStatus::SERVICE_NOT_FOUND;
    connectionStatusIndInd = std::make_shared<PiVisionConnectionStatusInd>(status, serviceNo, -1);
  }

  JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_STATUS_IND, connectionStatusIndInd);
}

void PiVisionEthTermConnectService::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_EVENT_SERVICE_DISCOVERY_TIMEOUT:
    {
      auto timeoutData = std::static_pointer_cast<PiVisionServiceDiscoveryTimeout>(dataPtr);
      if(timeoutData->serviceNo == serviceNo)
      {
        std::unique_lock<std::mutex> timeoutLock(timeoutMutex);
      	timeoutNotification.notify_one();
      }
      break;
    }
    default:
    break;
  }
}
