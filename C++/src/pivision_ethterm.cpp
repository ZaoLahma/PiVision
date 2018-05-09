#include "pivision_ethterm.h"
#include "jobdispatcher.h"
#include "pivision_threadmodel.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

PiVisionEthTermConnectServiceJob::PiVisionEthTermConnectServiceJob(const uint32_t _serviceNo):
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

void PiVisionEthTermConnectServiceJob::FindService(void)
{
  std::string serviceDiscoverString = serviceDiscoveryHeader + std::to_string(serviceNo);
  JobDispatcher::GetApi()->Log("serviceDiscoveryString: %s", serviceDiscoverString.c_str());

  struct sockaddr_in addr;

  memset(&addr,0,sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(multicastGroup.c_str());
  addr.sin_port = htons(serviceNo - 1u);

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
    JobDispatcher::GetApi()->Log("Received service address: %s",
                                 serverAddress.c_str());
    serviceFound = true;
  }
}

int32_t PiVisionEthTermConnectServiceJob::ConnectToServer()
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
      printf("getddrinfo\n");
      return -1;
  }

  for(p = servinfo; p != 0; p = p->ai_next)
  {
    (void) printf("Finding suitable servinfo\n");
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

void PiVisionEthTermConnectServiceJob::Execute()
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
    JobDispatcher::GetApi()->Log("ConnectServiceJob Service %u found", serviceNo);
    if(-1 != (socketFd = ConnectToServer()))
    {
      PiVisionServiceStatus status = PiVisionServiceStatus::SERVICE_CONNECTED;
      connectionStatusIndInd = std::make_shared<PiVisionConnectionStatusInd>(status, serviceNo, socketFd);
      JobDispatcher::GetApi()->Log("ConnectServiceJob PiVision connected to service %u", serviceNo);
    }
    else
    {
      JobDispatcher::GetApi()->Log("ConnectServiceJob Failed to connect to service %u", serviceNo);
      PiVisionServiceStatus status = PiVisionServiceStatus::SERVICE_DISCONNECTED;
      connectionStatusIndInd = std::make_shared<PiVisionConnectionStatusInd>(status, serviceNo, -1);
    }
  }
  else
  {
    JobDispatcher::GetApi()->Log("ConnectServiceJob Service %u not found", serviceNo);
    PiVisionServiceStatus status = PiVisionServiceStatus::SERVICE_NOT_FOUND;
    connectionStatusIndInd = std::make_shared<PiVisionConnectionStatusInd>(status, serviceNo, -1);
  }

  JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_SERVICE_STATUS_IND, connectionStatusIndInd);
}

void PiVisionEthTermConnectServiceJob::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
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

/* ------- PiVisionEthTermConnectionJob -------  */

PiVisionEthTermConnectionJob::PiVisionEthTermConnectionJob(const int32_t _socketFd) : socketFd(_socketFd)
{

}

void PiVisionEthTermConnectionJob::Receive(const uint32_t numBytesToGet, PiVisionDataBuf& dataBuf)
{
  const uint32_t MAX_CHUNK_SIZE = 256u;
  unsigned char buffer[MAX_CHUNK_SIZE];

  int32_t numBytesReceived = 0;
  while((uint32_t)numBytesReceived < numBytesToGet)
  {
    uint32_t maxChunkSize = std::min(MAX_CHUNK_SIZE, numBytesToGet - numBytesReceived);
    (void) memset(buffer, 0, sizeof(buffer));
    int32_t chunkSize = recv(socketFd,
                             buffer,
                             maxChunkSize,
                             0);

    if(chunkSize > 0)
    {
      numBytesReceived += chunkSize;
      for(uint32_t i = 0; i < (uint32_t)chunkSize; ++i)
      {
        dataBuf.push_back(buffer[i]);
      }
    }
    else
    {
      break;
    }
  }
}

void PiVisionEthTermConnectionJob::Execute()
{
  while(1)
  {
    PiVisionDataBuf dataBuf;

    const uint32_t PAYLOAD_SIZE_HEADER_SIZE = 4u;
    Receive(PAYLOAD_SIZE_HEADER_SIZE, dataBuf);

    char payloadSizeBuf[PAYLOAD_SIZE_HEADER_SIZE] = {0x0};
    for(uint32_t i = 0u; i < dataBuf.size(); ++i)
    {
      payloadSizeBuf[i] = dataBuf[i];
    }

    uint32_t payloadLength;
    (void) memcpy(&payloadLength, payloadSizeBuf, sizeof(payloadSizeBuf));

    dataBuf.clear();
    Receive(payloadLength, dataBuf);

    auto newFrameInd = std::make_shared<PiVisionNewFrameInd>(dataBuf);
    JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_NEW_FRAME_IND, newFrameInd);
  }
}

void PiVisionEthTermConnectionJob::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{

}

/* ------- PiVisionEthTerm ------- */

PiVisionEthTerm::PiVisionEthTerm()
{
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REQ, this);
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_SERVICE_STATUS_IND, this);
}

void PiVisionEthTerm::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    case PIVISION_EVENT_CONNECT_TO_SERVICE_REQ:
    {
      std::shared_ptr<PiVisionConnectToServiceReq> serviceReq = std::static_pointer_cast<PiVisionConnectToServiceReq>(dataPtr);
      std::shared_ptr<JobBase> serviceReqJob = std::make_shared<PiVisionEthTermConnectServiceJob>(serviceReq->serviceNo);
      JobDispatcher::GetApi()->ExecuteJobInGroup(serviceReqJob, PIVISION_SERVICE_DISCOVERY_THREAD_ID);
      break;
    }
    case PIVISION_EVENT_SERVICE_STATUS_IND:
    {
      std::shared_ptr<PiVisionConnectionStatusInd> statusInd = std::static_pointer_cast<PiVisionConnectionStatusInd>(dataPtr);
      switch(statusInd->status)
      {
        case PiVisionServiceStatus::SERVICE_NOT_FOUND:
        {
          std::shared_ptr<EventDataBase> connectRej = std::make_shared<PiVisionConnectToServiceRej>(statusInd->serviceNo);
          JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_REJ, connectRej);
          break;
        }
        case PiVisionServiceStatus::SERVICE_CONNECTED:
        {
          auto connectCfm = std::make_shared<PiVisionConnectToServiceCfm>(statusInd->serviceNo,
                                                                          (uint32_t)statusInd->socketFd);

          JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_CONNECT_TO_SERVICE_CFM, connectCfm);

          auto connectionJob = std::make_shared<PiVisionEthTermConnectionJob>(statusInd->socketFd);
          JobDispatcher::GetApi()->ExecuteJobInGroup(connectionJob, PIVISION_CONNECTIONS_THREAD_ID);
          break;
        }
        default:
        break;
      }
      break;
    }
    default:
      JobDispatcher::GetApi()->Log("PiVisionEthTerm received unexpected event: 0x%x", eventNo);
      break;
  }
}
