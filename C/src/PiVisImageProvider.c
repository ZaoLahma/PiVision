#include "PiVisImageProvider.h"
#include "PiVisScheduler.h"
#include "PiVisConstants.h"
#include "PiVisClient.h"
#include <string.h>
#include <stdio.h>

static SchdRunFuncEntry funcEntry;

static char recBuffer[COLOR_IMAGE_SIZE];
static unsigned int recBufferSize;

static void run(void);

static void run(void)
{
	recBufferSize = CLIENT_receive(recBuffer, (COLOR_IMAGE_SIZE));
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
	unsigned int bytesToCopy = 0u;

  if(recBufferSize != 0u)
  {
    bytesToCopy = bufSize;
  	if(bufSize > recBufferSize)
  	{
  		bytesToCopy = recBufferSize;
  	}
  	(void) memcpy(buf, recBuffer, bytesToCopy);
  }

	return bytesToCopy;
}
