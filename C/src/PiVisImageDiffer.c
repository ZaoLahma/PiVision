#include "../inc/PiVisImageDiffer.h"
#include "../inc/PiVisClient.h"
#include "../inc/PiVisScheduler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define COLOR_IMAGE_SIZE (640 * 480 * 3)

static SchdRunFuncEntry funcEntry;

static char recBuffer[COLOR_IMAGE_SIZE];
static char prevBuffer[(COLOR_IMAGE_SIZE)];

static void run(void);

static void run(void)
{
	if((COLOR_IMAGE_SIZE) == CLIENT_receive(recBuffer, (COLOR_IMAGE_SIZE)))
	{
		(void) printf("Finding diffs\n");
		unsigned int bufIndex = 0u;
		unsigned int colorIndex = 0u;
		unsigned int diff = 0;

		for(; bufIndex < (COLOR_IMAGE_SIZE); ++bufIndex)
		{
			diff += abs(prevBuffer[bufIndex] - recBuffer[bufIndex]);

			colorIndex += 1u;

			if(colorIndex > 2)
			{
				if(diff > 100)
				{
					(void) printf("Found diff: %u\n", diff);
				}
				colorIndex = 0;
				diff = 0u;
			}
		}

		memcpy(prevBuffer, recBuffer, (COLOR_IMAGE_SIZE));
	}
}


void IMAGEDIFF_init(void)
{
	funcEntry.run = run;
	funcEntry.next = 0;

	SCHED_registerCallback(&funcEntry);
}
