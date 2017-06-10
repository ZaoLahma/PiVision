#include "../inc/PiVisServer.h"
#include "../inc/PiVisScheduler.h"
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#define IP_ADDRESS_LENGTH INET_ADDRSTRLEN
#define INVALID_32_BIT_INT 0xFFFFFFFF
#define COLOR_PORT_NO (3070)
#define GRAY_PORT_NO (3071)
#define DISCOVER_COLOR_SERVICE (3069)
#define DISCOVER_COLOR_SERVICE_MESSAGE "WHERE_IS_3070"
#define DISCOVER_GRAY_SERVICE (3068)
#define DISCOVER_GRAY_SERVICE_MESSAGE "WHERE_IS_3071"
#define MULTICAST_GROUP "224.1.1.1"

static SchdRunFuncEntry funcEntry;
static char ownIpAddress[IP_ADDRESS_LENGTH];
static unsigned int servedPortNo;
static int serverSocket;
static int serviceDiscoverySocket;
static struct sockaddr_in addr;

static void getOwnIpAddress(char* address);
static void initiateServiceDiscoverySocket(void);

static void run(void);
static void handleNewConnections(void);
static void handleNewServiceDiscoveryRequests(void);

static void getOwnIpAddress(char* address)
{
	printf("getOwnIpAddress called\n");

	struct ifaddrs* addrs;
	int retVal = getifaddrs(&addrs);

	(void) printf("retVal: %d\n", retVal);

	struct ifaddrs* tmp = addrs;

	while(tmp)
	{
		printf("In tmp\n");
	    if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
	    {
	        struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
	        inet_ntop(AF_INET, &((*pAddr).sin_addr), address, IP_ADDRESS_LENGTH);
	        if(strncmp("127.0.0.1", address, IP_ADDRESS_LENGTH) != 0)
	        {
	        	break;
	        }
	    }

	    tmp = tmp->ifa_next;
	}

	(void) printf("Serving address %s\n", address);

	freeifaddrs(addrs);
}

static void initiateServiceDiscoverySocket(void)
{
	unsigned int toServe = INVALID_32_BIT_INT;

	if(servedPortNo == COLOR_PORT_NO)
	{
		toServe = DISCOVER_COLOR_SERVICE;
	}
	else if(servedPortNo == GRAY_PORT_NO)
	{
		toServe = DISCOVER_GRAY_SERVICE;
	}

	printf("toServe: %u\n", toServe);

    int nbytes;
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
    addr.sin_port = htons(toServe);

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
	timeout.tv_usec = 1000;

	setsockopt(serviceDiscoverySocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
}

static void run()
{
	(void) printf("Server run called\n");
	handleNewServiceDiscoveryRequests();
}

static void handleNewServiceDiscoveryRequests()
{
	char messageBuf[50];
	unsigned int addrLen = sizeof(addr);
	int bytesReceived = recvfrom(serviceDiscoverySocket,
								 messageBuf,
								 sizeof(messageBuf),
								 0,
								 (struct sockaddr *)&addr,
								 &addrLen);

	printf("messageBuf: %s\n", messageBuf);

	messageBuf[bytesReceived] = '\0';

	char header[] = "WHERE_IS_";

	if((unsigned int)bytesReceived > strlen(header))
	{
		char* portNoStr = &messageBuf[strlen(header)];
		unsigned int portNo = atoi(portNoStr);

		if(servedPortNo == portNo)
		{
			printf("Service provided. Responding to: %s\n", inet_ntoa(addr.sin_addr));
			sendto(serviceDiscoverySocket, ownIpAddress, IP_ADDRESS_LENGTH, 0, (struct sockaddr*)&addr, sizeof(addr));
		}
		else
		{
			printf("Service not provided\n");
		}
	}

}

void SERVER_init()
{
	funcEntry.run = run;
	funcEntry.next = 0;

	getOwnIpAddress(ownIpAddress);

	servedPortNo = (INVALID_32_BIT_INT);

	SCHED_registerCallback(&funcEntry);
}

void SERVER_publishService(unsigned int portNo)
{
	servedPortNo = portNo;

	initiateServiceDiscoverySocket();
}

void SERVER_send(char* buf, unsigned int size)
{

}
