#include "PiVisClient.h"
#include "PiVisScheduler.h"
#include "PiVisConstants.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

typedef void (*StateFunc)(void);
typedef struct CLIENT_state
{
	char* state;
	StateFunc stateFunc;
} CLIENT_state;

static int connect_to_server(char* address, char* portNo);
static int socket_receive(int fileDesc, void* data, int max_size);
static char serverAddress[IP_ADDRESS_LENGTH];

static void serviceDiscovery(void);
static void stateDisconnected(void);
static void stateConnecting(void);
static void stateConnected(void);
static void processingFrame(void);
static void run(void);

static unsigned int currState;

static int socketFd = -1;
static int serviceDiscoverySocket = -1;
static char camBufHeader[CAMERA_BUF_HEADER_SIZE] = {0u};
static char buffer[(COLOR_IMAGE_SIZE)];
static unsigned int bufferIndex = 0u;
static unsigned int headerIndex = 0u;

static CLIENT_state state[] =
{
		{ "SERVICE_DISCOVERY", serviceDiscovery },
		{ "DISCONNECTED", stateDisconnected },
		{ "CONNECTING", stateConnecting },
		{ "CONNECTED", stateConnected },
		{ "PROCESSING_FRAME", processingFrame }
};

static SchdRunFuncEntry funcEntry;

static void stateDisconnected(void)
{
	currState += 1u;
}

static void serviceDiscovery(void)
{
    struct sockaddr_in addr;

    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    addr.sin_port = htons(DISCOVER_COLOR_SERVICE);

	char message[] = DISCOVER_COLOR_SERVICE_MESSAGE;

	int numBytesSent = sendto(serviceDiscoverySocket, message, sizeof(message), 0, (struct sockaddr*)&addr, sizeof(addr));
	if(numBytesSent < 0)
	{
 	    perror("sendto");
	    exit(1);
	}

	int bytesReceived = recv(serviceDiscoverySocket, serverAddress, sizeof(serverAddress), 0);

	if(bytesReceived > 0)
	{
		serverAddress[bytesReceived] = '\0';
		(void) printf("Received: %s\n", serverAddress);
		currState += 1u;
	}
	else
	{
		usleep(1000000);
	}
}

static void stateConnecting(void)
{
	if(-1 != (socketFd = connect_to_server(serverAddress, "3070")))
	{
		currState += 1u;
	}
}

static void stateConnected(void)
{
  if(headerIndex < CAMERA_BUF_HEADER_SIZE)
  {
    int numBytesReceived = socket_receive(socketFd, &camBufHeader[headerIndex], CAMERA_BUF_HEADER_SIZE);

    if(-1 != numBytesReceived)
    {
      headerIndex += numBytesReceived;
      (void) printf("headerIndex: %u\n", headerIndex);
    }
  }
  else
  {
  	int numBytesReceived = socket_receive(socketFd, &buffer[bufferIndex], (COLOR_IMAGE_SIZE) - bufferIndex);

  	if(-1 != numBytesReceived)
  	{
  		bufferIndex += numBytesReceived;
  	}
  	else
  	{
  		currState = 0u;
  		memset(buffer, 0u, sizeof(buffer));
  	}

  	if(bufferIndex == (COLOR_IMAGE_SIZE))
  	{
  		bufferIndex = 0u;
      headerIndex = 0u;
  		currState += 1u;
  	}
  }
}

static void processingFrame(void)
{
	if(bufferIndex == (COLOR_IMAGE_SIZE))
	{
		bufferIndex = 0u;
		currState -= 1u;
	}
}

static int connect_to_server(char* address, char* portNo)
{
    int sockfd;

    struct addrinfo hints;
    struct addrinfo* servinfo;
    struct addrinfo* p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(address, portNo, &hints, &servinfo) != 0)
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

static int socket_receive(int fileDesc, void* data, int max_size)
{
    return recv(fileDesc, data, max_size, 0);
}

static void run(void)
{
	state[currState].stateFunc();
}

void CLIENT_init(void)
{
	serviceDiscoverySocket = socket(AF_INET, SOCK_DGRAM, 0);

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(serviceDiscoverySocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

	memset(serverAddress, 0, IP_ADDRESS_LENGTH);

	funcEntry.run = run;
	funcEntry.next = 0;

	SCHED_registerCallback(&funcEntry);
}

unsigned int CLIENT_receive(char* buf, unsigned int bufSize)
{
	unsigned int retVal = 0u;
	if(4u == currState)
	{
		(void) memcpy(buf, &buffer[bufferIndex], bufSize);
		retVal = bufSize;
		bufferIndex += retVal;
	}

	return retVal;
}
