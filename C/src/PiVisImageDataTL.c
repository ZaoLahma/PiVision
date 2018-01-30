#include "PiVisImageDataTL.h"
#include "PiVisServer.h"
#include "PiVisConstants.h"
#include "PiVisScheduler.h"
#include <string.h>

#define ACK_BUF_SIZE    1u

#define IMAGE_TYPE_HEADER_OFFSET   0u
#define IMAGE_SIZE_HEADER_OFFSET   1u
#define IMAGE_X_SIZE_HEADER_OFFSET 5u
#define IMAGE_Y_SIZE_HEADER_OFFSET 7u
#define BUF_HEADER_SIZE            9u /* 1 + 4 + 2 + 2 */
#define GRAY_SCALE_IMAGE_HEADER    0x0
#define COLOR_IMAGE_HEADER         0x1

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

void IMGDATATL_sendGrayscaleImage(char* buf, unsigned int size, unsigned short xSize, unsigned short ySize)
{
	//TODO: Fill header with relevant data before sending

	if(SEND_DATA == state)
	{
		const unsigned int totSize = BUF_HEADER_SIZE + size;
		char bufToSend[totSize];
		memset(bufToSend, 0, sizeof(bufToSend));

		bufToSend[IMAGE_TYPE_HEADER_OFFSET] = GRAY_SCALE_IMAGE_HEADER;

		(void) memcpy(&bufToSend[IMAGE_SIZE_HEADER_OFFSET],   &size,  sizeof(size));
		(void) memcpy(&bufToSend[IMAGE_X_SIZE_HEADER_OFFSET], &xSize, sizeof(xSize));
		(void) memcpy(&bufToSend[IMAGE_Y_SIZE_HEADER_OFFSET], &xSize, sizeof(ySize));

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
