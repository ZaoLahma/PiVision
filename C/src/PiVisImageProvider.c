#include "../inc/PiVisImageProvider.h"
#include "../inc/PiVisScheduler.h"
#include "../inc/PiVisConstants.h"
#include "../inc/PiVisClient.h"
#include <string.h>
#include <stdio.h>

static SchdRunFuncEntry funcEntry;

static unsigned char recBuffer[COLOR_IMAGE_SIZE];
static unsigned int  recBufferSize;

static void run(void);

static void run(void)
{
	if((COLOR_IMAGE_SIZE) == CLIENT_receive(recBuffer, (COLOR_IMAGE_SIZE)))
	{
		recBufferSize = (COLOR_IMAGE_SIZE);
	}
}

void IMGPROVIDER_init(void)
{
	recBufferSize = 0u;

	funcEntry.run = run;
	funcEntry.next = 0;

	SCHED_registerCallback(&funcEntry);
}

int IMGPROVIDER_getPixelData(unsigned char* buf, unsigned int bufSize)
{
	unsigned int bytesToCopy = bufSize;

	if(bufSize > recBufferSize)
	{
		bytesToCopy = recBufferSize;
	}

	(void) printf("bytesToCopy: %u\n", bytesToCopy);

	(void) memcpy(buf, recBuffer, bytesToCopy);

	return bytesToCopy;
}
