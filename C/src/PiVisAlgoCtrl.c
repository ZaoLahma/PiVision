#include "../inc/PiVisAlgoCtrl.h"
#include "../inc/PiVisScheduler.h"
#include "../inc/PiVisConstants.h"
#include "../inc/PiVisImageProvider.h"
#include "../inc/PiVisImageDataTL.h"
#include "external/thread_pool.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NUM_ALGORITHMS 1u

typedef enum
{
	ALGO_INIT,
	ALGO_RUNNING,
	ALGO_COMPLETE
} PiVisAlgoState;

typedef struct
{
	PiVisAlgoState state;
	char* inputData;
	char* outputData;
	unsigned int outputDataSize;
} PiVisAlgoContext;

typedef struct
{
	PiVisAlgoContext algorithms[NUM_ALGORITHMS];
} PiVisAlgoControlContext;

static PiVisAlgoControlContext algoControlContext;

static SchdRunFuncEntry funcEntry;

static struct ThreadContext* threadContext;

static char recBuffer[(COLOR_IMAGE_SIZE)];
static char prevBuffer[(COLOR_IMAGE_SIZE)];
static char imageToSend[(GRAYSCALE_IMAGE_SIZE)];

static unsigned char receivedFirstImage = 0u;

static void* imageDifferFunc(void* arg);

static void run(void);

static void* imageDifferFunc(void* arg)
{
	PiVisAlgoContext* algoContext = (PiVisAlgoContext*)(arg);
	algoContext->outputData = imageToSend;
	algoContext->outputDataSize = (GRAYSCALE_IMAGE_SIZE);

	unsigned int bufIndex = 0u;
	unsigned int colorIndex = 0u;
	unsigned int diff = 0u;
	unsigned int numDiffs = 0u;

	unsigned int toSendPixelIndex = 0u;

	unsigned int colorIntensity = 0u;

	if(1u == receivedFirstImage)
	{
		for(; bufIndex < (COLOR_IMAGE_SIZE); ++bufIndex)
		{
			diff += abs(prevBuffer[bufIndex] - algoContext->inputData[bufIndex]);

			colorIndex += 1u;
			colorIntensity += algoContext->inputData[bufIndex];

			if(colorIndex > 2)
			{
				if(diff > 100)
				{
					/*
					(void) printf("Found diff: %u. Buffers R: %u - %u, G: %u - %u, B: %u - %u\n",
							diff,
							prevBuffer[bufIndex - 2u],
							recBuffer[bufIndex - 2u],
							prevBuffer[bufIndex - 1u],
							recBuffer[bufIndex - 1u],
							prevBuffer[bufIndex],
							recBuffer[bufIndex]);
							*/
					numDiffs += 1u;
					imageToSend[toSendPixelIndex] = 255u;
				}
				else
				{
					imageToSend[toSendPixelIndex] = colorIntensity / 3u;
				}
				colorIndex = 0;
				diff = 0u;
				toSendPixelIndex += 1u;
				colorIntensity = 0u;
			}
		}
	}

	receivedFirstImage = 1u;

	(void) memcpy(prevBuffer, algoContext->inputData, (COLOR_IMAGE_SIZE));

	algoContext->state = ALGO_COMPLETE;

	(void) printf("Algo complete\n");

	return 0;
}

static void run(void)
{
	if(ALGO_COMPLETE == algoControlContext.algorithms[0].state)
	{
		IMGDATATL_sendGrayscaleImage(algoControlContext.algorithms[0].outputData,
									 algoControlContext.algorithms[0].outputDataSize,
									 (IMAGE_X_SIZE),
									 (IMAGE_Y_SIZE));
	}

	if((COLOR_IMAGE_SIZE) == IMGPROVIDER_getPixelData(recBuffer, (COLOR_IMAGE_SIZE)))
	{
		if(ALGO_RUNNING != algoControlContext.algorithms[0].state)
		{
			algoControlContext.algorithms[0].state = ALGO_RUNNING;
			algoControlContext.algorithms[0].inputData = recBuffer;

			sched_job(threadContext, imageDifferFunc, &algoControlContext.algorithms[0]);

			(void) printf("Algo running\n");
		}
		else
		{
			(void) printf("Dropping frame from camera\n");
		}
	}
}


void ALGOCTRL_init(void)
{
	funcEntry.run = run;
	funcEntry.next = 0;

	SCHED_registerCallback(&funcEntry);

	algoControlContext.algorithms[0].state = ALGO_INIT;

	threadContext = init_thread_pool((NUM_ALGORITHMS));
}
