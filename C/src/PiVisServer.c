#include "../inc/PiVisServer.h"
#include "../inc/PiVisScheduler.h"
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define IP_ADDRESS_LENGTH INET_ADDRSTRLEN
#define INVALID_32_BIT_INT 0xFFFFFFFF

static SchdRunFuncEntry funcEntry;
static char ownIpAddress[IP_ADDRESS_LENGTH];
static unsigned int servedPortNo;
static int socketFd;

static void getOwnIpAddress(char* address);
static void initiateSocket(void);

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

static void run()
{
	(void) printf("Server run called\n");
}

void SERVER_init()
{
	funcEntry.run = run;
	funcEntry.next = 0;

	getOwnIpAddress(ownIpAddress);

	servedPortNo = (INVALID_32_BIT_INT);

	SCHED_registerCallback(&funcEntry);
}

SERVER_setServedPortNo(unsigned int portNo)
{
	servedPortNo = portNo;
}

void SERVER_send(char* buf, unsigned int size)
{

}
