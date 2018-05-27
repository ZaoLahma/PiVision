#include "PiVisImageDataTL.h"
#include "PiVisServer.h"
#include "PiVisConstants.h"
#include "PiVisScheduler.h"
#include <string.h>

#define ACK_BUF_SIZE            (5u)
#define ACK_BUF_FRAME_NO_OFFSET (4u)

#define IMAGE_SIZE_HEADER_OFFSET   0u
#define IMAGE_X_SIZE_HEADER_OFFSET 4u
#define IMAGE_Y_SIZE_HEADER_OFFSET 6u
#define BUF_HEADER_SIZE            4u /* 4 */

#define UNUSED_ARG(arg) arg = arg

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
static unsigned char ackBuf[ACK_BUF_SIZE];

static void run(void);

static void run(void)
{
	int receivedBytes = SERVER_receive(&serverContext, (unsigned char*)&ackBuf, (ACK_BUF_SIZE));

	if(-1 != receivedBytes)
	{
		if(ackBuf[ACK_BUF_FRAME_NO_OFFSET] == frameNo)
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

void IMGDATATL_sendGrayscaleImage(unsigned char* buf, unsigned int size, unsigned short xSize, unsigned short ySize)
{
  UNUSED_ARG(xSize);
  UNUSED_ARG(ySize);

	if(SEND_DATA == state)
	{
		const unsigned int totSize = BUF_HEADER_SIZE + size;
		unsigned char bufToSend[totSize];
		memset(bufToSend, 0, sizeof(bufToSend));

		(void) memcpy(&bufToSend[IMAGE_SIZE_HEADER_OFFSET],   &size,  sizeof(size));

		(void) memcpy(&bufToSend[BUF_HEADER_SIZE], buf, size);

		int sendStatus = SERVER_send(&serverContext, bufToSend, totSize);

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
