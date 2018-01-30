#include "PiVisAlgoCtrl.h"
#include "PiVisScheduler.h"
#include "PiVisConstants.h"
#include "PiVisImageProvider.h"
#include "PiVisImageDataTL.h"
#include "PiVisImageDiffAlgo.h"
#include "external/thread_pool.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NUM_ALGORITHMS 1u

typedef struct
{
	PiVisAlgoContext algorithms[NUM_ALGORITHMS];
} PiVisAlgoControlContext;

static PiVisAlgoControlContext algoControlContext;
static SchdRunFuncEntry funcEntry;
static struct ThreadContext* threadContext;
static char recBuffer[(COLOR_IMAGE_SIZE)];

static void run(void);

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

			sched_job(threadContext, algoControlContext.algorithms[0].exec, &algoControlContext.algorithms[0]);

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
  algoControlContext.algorithms[0].exec = &IMGDIFFALGO_exec;

	threadContext = init_thread_pool((NUM_ALGORITHMS));
}
