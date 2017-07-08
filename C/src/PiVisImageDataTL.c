#include "../inc/PiVisImageDataTL.h"
#include "../inc/PiVisServer.h"
#include "../inc/PiVisConstants.h"
#include "../inc/PiVisScheduler.h"
#include <string.h>

#define ACK_BUF_SIZE 1

enum PiVisImageDataTLState
{
	SEND_DATA,
	WAIT_FOR_ACK
};

static SchdRunFuncEntry funcEntry;
static PiVisServerContext serverContext;
static enum PiVisImageDataTLState state;

static unsigned int numDroppedFrames;
static unsigned char frameNo;
static unsigned char ackBuf;

static void run(void);

static void run(void)
{
	int receivedBytes = SERVER_receive(&serverContext, (char*)&ackBuf, (ACK_BUF_SIZE));

	if(-1 != receivedBytes)
	{
		if(ackBuf == frameNo)
		{
			state = SEND_DATA;
			frameNo += 1;
		}
	}
}

void IMGDATATL_init(void)
{
	serverContext.servedPortNo = (GRAY_PORT_NO);
	serverContext.serviceDiscoveryPortNo = (DISCOVER_GRAY_SERVICE);

	SERVER_publishService(&serverContext);

	funcEntry.run = run;
	funcEntry.next = 0;

	SCHED_registerCallback(&funcEntry);

	state = SEND_DATA;

	numDroppedFrames = 0;
	frameNo = 0;
}

void IMGDATATL_send(char* buf, unsigned int size)
{
	//TODO: Fill header with relevant data before sending
	if(SEND_DATA == state)
	{
		int sendStatus = SERVER_send(&serverContext, buf, size);

		if(-1 != sendStatus)
		{
			state = WAIT_FOR_ACK;
		}
		else
		{
			numDroppedFrames++;
		}
	}
	else
	{
		numDroppedFrames++;
	}
}

unsigned int IMGDATATL_getNumDroppedFrames(void)
{
	return numDroppedFrames;
}
