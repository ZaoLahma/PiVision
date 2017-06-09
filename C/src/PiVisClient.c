#include "../inc/PiVisClient.h"
#include "../inc/PiVisScheduler.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define COLOR_IMAGE_SIZE (640 * 480 * 3)
#define DISCOVER_COLOR_SERVICE (3069)
#define DISCOVER_COLOR_SERVICE_MESSAGE "WHERE_IS_3070"
#define MULTICAST_GROUP "224.1.1.1"

typedef void (*StateFunc)(void);
typedef struct CLIENT_state
{
	char* state;
	StateFunc stateFunc;
} CLIENT_state;

static int connect_to_server(char* address, char* portNo);
static int socket_receive(int fileDesc, void* data, int max_size);
static void serviceDiscovery(void);
static void stateDisconnected(void);
static void stateConnecting(void);
static void stateConnected(void);
static void processingFrame(void);
static void run(void);

static unsigned int currState;

static int socketFd = -1;
static int serviceDiscoverySocket = -1;
static char buffer[(COLOR_IMAGE_SIZE)];
static unsigned int bufferIndex = 0;

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

	usleep(1000000);
}

static void stateConnecting(void)
{
	if(-1 != (socketFd = connect_to_server("192.168.1.106", "3070")))
	{
		currState += 1u;
	}
}

static void stateConnected(void)
{
	int numBytesReceived = socket_receive(socketFd, &buffer[bufferIndex], (COLOR_IMAGE_SIZE));

	if(-1 != numBytesReceived)
	{
		bufferIndex += numBytesReceived;
		(void) printf("bufferIndex: %u\n", bufferIndex);
	}
	else
	{
		currState = 0u;
		memset(buffer, 0u, sizeof(buffer));
	}

	if(bufferIndex == (COLOR_IMAGE_SIZE))
	{
		(void) printf("Received full image\n");
		bufferIndex = 0;
		currState += 1u;
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
    	(void) printf("Finding suitablt servinfo\n");
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
        else
        {
        	(void) printf("Connect failed\n");
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
	(void) printf("Handling state: %s\n", state[currState].state);

	state[currState].stateFunc();
}

void CLIENT_init(void)
{
	serviceDiscoverySocket = socket(AF_INET, SOCK_DGRAM, 0);

	funcEntry.run = run;
	funcEntry.next = 0;

	SCHED_registerCallback(&funcEntry);
}

int CLIENT_receive(char* buf, unsigned int bufSize)
{
	int retVal = -1;
	if(3u == currState)
	{
		(void) memcpy(buf, &buffer[bufferIndex], bufSize);
		retVal = bufSize;
		bufferIndex += retVal;

		(void) printf("bufferIndex: %u, bufSize: %u, COLOR_IMAGE_SIZE: %u\n",
				bufferIndex,
				bufSize,
				(COLOR_IMAGE_SIZE));
	}

	return retVal;
}
