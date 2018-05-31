#include "pivision_ethtermservicelistener.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_macros.h"
#include "pivision_threadmodel.h"
#include "pivision_ethtermconnection.h"

#include <sys/types.h>
#include <ifaddrs.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>

PiVisionEthTermServiceListener::PiVisionEthTermServiceListener(const uint32_t _serviceNo) :
serviceNo(_serviceNo),
active(true),
serviceDiscoverySocket(-1),
serverSocket(-1)
{
  JobDispatcher::GetApi()->SubscribeToEvent(PIVISION_EVENT_STOP, this);
  getOwnIpAddress();
}

PiVisionEthTermServiceListener::~PiVisionEthTermServiceListener()
{
  JobDispatcher::GetApi()->UnsubscribeToEvent(PIVISION_EVENT_STOP, this);
}

void PiVisionEthTermServiceListener::getOwnIpAddress()
{
  struct ifaddrs* addrs;
	(void) getifaddrs(&addrs);
	struct ifaddrs* tmp = addrs;

	while(tmp)
	{
	    if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
	    {
	        struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
	        inet_ntop(AF_INET, &((*pAddr).sin_addr), ownIpAddress, INET_ADDRSTRLEN);
	        if(strncmp("127.0.0.1", ownIpAddress, INET_ADDRSTRLEN) != 0)
	        {
	        	break;
	        }
	    }

	    tmp = tmp->ifa_next;
	}

	freeifaddrs(addrs);
}

void PiVisionEthTermServiceListener::initiateServiceDiscoverySocket()
{
  struct ip_mreq mreq;
  serviceDiscoverySocket = socket(AF_INET, SOCK_DGRAM, 0);
  uint32_t yes = 1;
  if(0 > setsockopt(serviceDiscoverySocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
  {
  	perror("setsockopt 1");
  	exit(1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(serviceNo);

  if(0 > bind(serviceDiscoverySocket, (struct sockaddr*)&addr, sizeof(addr)))
  {
  	perror("bind");
  	exit(1);
  }

  mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  if(0 != setsockopt(serviceDiscoverySocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)))
  {
    perror("setsockopt");
    exit(1);
  }

  struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1;

	setsockopt(serviceDiscoverySocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
}

void PiVisionEthTermServiceListener::initiateServerSocketFd()
{
  struct addrinfo hints;
  struct addrinfo* servinfo;
  struct addrinfo* p;
  int yes = 1;
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  char portNoStr[6] = "";

  sprintf(portNoStr, "%d", serviceNo);

  if ((rv = getaddrinfo(0, portNoStr, &hints, &servinfo)) != 0)
  {
    printf("portNo: %u\n", serviceNo);
    perror("getaddrinfo \n");
    exit(1);
  }


  for(p = servinfo; p != 0; p = p->ai_next)
  {
      if ((serverSocket = socket(p->ai_family, p->ai_socktype,
                           p->ai_protocol)) == -1)
      {
          continue;
      }

      if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes,
                     sizeof(int)) == -1)
      {
        perror("reuseaddr\n");
          exit(1);
      }

      if (bind(serverSocket, p->ai_addr, p->ai_addrlen) == -1)
      {
          close(serverSocket);
          continue;
      }

      break;
  }

  freeaddrinfo(servinfo);

  if (p == 0)
  {
    perror("p == 0\n");
      exit(1);
  }

  if (listen(serverSocket, 10) == -1)
  {
    perror("listen\n");
      exit(1);
  }

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 500000;

  setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
}

void PiVisionEthTermServiceListener::handleNewServiceDiscoveryRequests()
{
	char messageBuf[50];
	uint32_t addrLen = sizeof(addr);
	int bytesReceived = recvfrom(serviceDiscoverySocket,
              								 messageBuf,
              								 sizeof(messageBuf),
              								 0,
              								 (struct sockaddr *)&addr,
              								 &addrLen);

	char header[] = "WHERE_IS_";

	if(bytesReceived > (int)strlen(header))
	{
		messageBuf[bytesReceived] = '\0';

		char* portNoStr = &messageBuf[strlen(header)];
		uint32_t portNo = atoi(portNoStr);

		if(serviceNo == portNo)
		{
			JobDispatcher::GetApi()->Log("Service %u provided. Responding to: %s", portNo, inet_ntoa(addr.sin_addr));
			sendto(serviceDiscoverySocket, ownIpAddress, INET_ADDRSTRLEN, 0, (struct sockaddr*)&addr, sizeof(addr));
		}
		else
		{
			JobDispatcher::GetApi()->Log("Service %u not provided", portNo);
		}
	}
}

void PiVisionEthTermServiceListener::handleNewConnections()
{
  struct timeval tv;
  fd_set acceptFds;

  struct sockaddr_storage their_addr;
  socklen_t sin_size;

  sin_size = sizeof their_addr;

	FD_ZERO(&acceptFds);
	FD_SET(serverSocket, &acceptFds);

  tv.tv_sec = 0;
  tv.tv_usec = 500000;

	select(serverSocket + 1, &acceptFds, 0, 0, &tv);

	if (FD_ISSET(serverSocket, &acceptFds))
	{
		int32_t clientSocket = accept(serverSocket, (struct sockaddr *)&their_addr, &sin_size);
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 500000;
		setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    auto newConnection = std::make_shared<PiVisionEthTermConnection>(PiVisionConnectionType::PIVISION_SERVER,
                                                                     serviceNo,
                                                                     clientSocket);
    JobDispatcher::GetApi()->ExecuteJobInGroup(newConnection, PIVISION_CONNECTIONS_THREAD_ID);
	}
}

void PiVisionEthTermServiceListener::Execute()
{
  initiateServiceDiscoverySocket();
  initiateServerSocketFd();
  JobDispatcher::GetApi()->Log("Service %u published to network at address %s", serviceNo, ownIpAddress);
  while(active)
  {
    handleNewServiceDiscoveryRequests();
    handleNewConnections();
  }
}

void PiVisionEthTermServiceListener::HandleEvent(const uint32_t eventNo, std::shared_ptr<EventDataBase> dataPtr)
{
  PIVISION_UNUSED_ARG(dataPtr);

  switch(eventNo)
  {
    case PIVISION_EVENT_STOP:
    active = false;
    break;
    default:
    JobDispatcher::GetApi()->Log("PiVisionEthTermServiceListener received unexpected event: 0x%X", eventNo);
    break;
  }
}
