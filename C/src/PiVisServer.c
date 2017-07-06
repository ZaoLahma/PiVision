#include "../inc/PiVisServer.h"
#include "../inc/PiVisScheduler.h"
#include "../inc/PiVisConstants.h"
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>

static SchdRunFuncEntry funcEntry;
static char ownIpAddress[IP_ADDRESS_LENGTH];
static struct sockaddr_in addr;
static struct PiVisServerContext* connections;

static void getOwnIpAddress(char* address);
static void initiateServiceDiscoverySocket(PiVisServerContext* context);
static void initiateServerSocketFd(PiVisServerContext* context);

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

static void initiateServerSocketFd(PiVisServerContext* context)
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

    sprintf(portNoStr, "%d", context->servedPortNo);

    if ((rv = getaddrinfo(0, portNoStr, &hints, &servinfo)) != 0)
    {
    	printf("portNo: %u\n", context->servedPortNo);
    	perror("getaddrinfo \n");
    	exit(1);
    }


    for(p = servinfo; p != 0; p = p->ai_next)
    {
        if ((context->serverContext.serverSocket = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            continue;
        }

        if (setsockopt(context->serverContext.serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
        	perror("reuseaddr\n");
            exit(1);
        }

        if (bind(context->serverContext.serverSocket, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(context->serverContext.serverSocket);
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

    if (listen(context->serverContext.serverSocket, 10) == -1)
    {
    	perror("listen\n");
        exit(1);
    }

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;

	setsockopt(context->serverContext.serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
}

static void initiateServiceDiscoverySocket(PiVisServerContext* context)
{
    struct ip_mreq mreq;
    context->serverContext.serviceDiscoverySocket = socket(AF_INET, SOCK_DGRAM, 0);
	unsigned int yes = 1;
    if(0 > setsockopt(context->serverContext.serviceDiscoverySocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
    {
    	perror("setsockopt 1");
    	exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(context->serviceDiscoveryPortNo);

    if(0 > bind(context->serverContext.serviceDiscoverySocket, (struct sockaddr*)&addr, sizeof(addr)))
    {
    	perror("bind");
    	exit(1);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    setsockopt(context->serverContext.serviceDiscoverySocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1;

	setsockopt(context->serverContext.serviceDiscoverySocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
}

static void run()
{
	handleNewServiceDiscoveryRequests();
	handleNewConnections();
}

static void handleNewConnections()
{
    struct timeval tv;
    fd_set acceptFds;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    struct sockaddr_storage their_addr;
    socklen_t sin_size;

    sin_size = sizeof their_addr;

	PiVisServerContext* context = (PiVisServerContext*)connections;

	while(context != 0)
	{
		FD_ZERO(&acceptFds);
		FD_SET(context->serverContext.serverSocket, &acceptFds);

		select(context->serverContext.serverSocket + 1, &acceptFds, 0, 0, &tv);

		if (FD_ISSET(context->serverContext.serverSocket, &acceptFds))
		{
			context->serverContext.clientSocket = accept(context->serverContext.serverSocket, (struct sockaddr *)&their_addr, &sin_size);
			context->connected = 1u;
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 1;
			setsockopt(context->serverContext.clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
			(void) printf("Accepted new connection\n");
		}
		context = (PiVisServerContext*)context->next;
	}
}

static void handleNewServiceDiscoveryRequests()
{
	PiVisServerContext* context = (PiVisServerContext*)connections;

	while(context != 0)
	{
		if(0 == context->connected)
		{
			char messageBuf[50];
			unsigned int addrLen = sizeof(addr);
			int bytesReceived = recvfrom(context->serverContext.serviceDiscoverySocket,
										 messageBuf,
										 sizeof(messageBuf),
										 0,
										 (struct sockaddr *)&addr,
										 &addrLen);

			messageBuf[bytesReceived] = '\0';

			char header[] = "WHERE_IS_";

			if(bytesReceived > (int)strlen(header))
			{
				char* portNoStr = &messageBuf[strlen(header)];
				unsigned int portNo = atoi(portNoStr);

				if(context->servedPortNo == portNo)
				{
					printf("Service provided. Responding to: %s\n", inet_ntoa(addr.sin_addr));
					sendto(context->serverContext.serviceDiscoverySocket, ownIpAddress, IP_ADDRESS_LENGTH, 0, (struct sockaddr*)&addr, sizeof(addr));
				}
				else
				{
					printf("Service not provided\n");
				}
			}
		}
		context = (PiVisServerContext*)context->next;
	}
}

void SERVER_init()
{
	connections = 0;

	funcEntry.run = run;
	funcEntry.next = 0;

	getOwnIpAddress(ownIpAddress);

	SCHED_registerCallback(&funcEntry);
}

void SERVER_publishService(PiVisServerContext* context)
{
	(void) printf("Published service pointer %p at portNo: %u, serviceDiscovery: 0x%x\n",
			      context,
				  context->servedPortNo,
				  context->serviceDiscoveryPortNo);

	context->next = 0;
	context->connected = 0u;
	context->serverContext.clientSocket = (INVALID_32_BIT_INT);
	if(0 == connections)
	{
		connections = (struct PiVisServerContext*)context;
	}
	else
	{
		PiVisServerContext* last = (PiVisServerContext*)connections;
		while(last->next != 0)
		{
			last = (PiVisServerContext*)last->next;
		}
		last->next = (struct PiVisServerContext*)context;
	}
	initiateServiceDiscoverySocket(context);
	initiateServerSocketFd(context);
}

void SERVER_send(PiVisServerContext* context, char* buf, unsigned int size)
{
	if((int)(INVALID_32_BIT_INT) != context->serverContext.clientSocket)
	{
		int sentBytes = 0;

		while(sentBytes < (int)size)
		{
			sentBytes += send(context->serverContext.clientSocket, buf, size, 0);
		}
	}
}

int SERVER_receive(PiVisServerContext* context, char* buf, unsigned int bufSize)
{
	int numBytesReceived = -1;

	if(0u != context->connected)
	{
		numBytesReceived = recv(context->serverContext.clientSocket, buf, bufSize, 0);
	}

	return numBytesReceived;
}
