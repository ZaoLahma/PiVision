#include "pivision_ethtermservicelistener.h"
#include "jobdispatcher.h"
#include "pivision_events.h"
#include "pivision_macros.h"

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
serviceDiscoverySocket(-1)
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

int PiVisionEthTermServiceListener::initiateServiceDiscoverySocket()
{
  struct ip_mreq mreq;
  serviceDiscoverySocket = socket(AF_INET, SOCK_DGRAM, 0);
  unsigned int yes = 1;
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

  setsockopt(serviceDiscoverySocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

  struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1;

	setsockopt(serviceDiscoverySocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
  return 0;
}

void PiVisionEthTermServiceListener::handleNewServiceDiscoveryRequests()
{
	char messageBuf[50];
	unsigned int addrLen = sizeof(addr);
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
		unsigned int portNo = atoi(portNoStr);

		if(serviceNo == portNo)
		{
			printf("Service provided. Responding to: %s\n", inet_ntoa(addr.sin_addr));
			sendto(serviceDiscoverySocket, ownIpAddress, INET_ADDRSTRLEN, 0, (struct sockaddr*)&addr, sizeof(addr));
		}
		else
		{
			printf("Service not provided\n");
		}
	}
}

void PiVisionEthTermServiceListener::Execute()
{
  if(0u == initiateServiceDiscoverySocket())
  {
    JobDispatcher::GetApi()->Log("Service %u published to network at address %s", serviceNo, ownIpAddress);
    while(active)
    {
      handleNewServiceDiscoveryRequests();
    }
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
