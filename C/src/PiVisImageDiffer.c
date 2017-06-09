#include "../inc/PiVisImageDiffer.h"
#include "../inc/PiVisClient.h"
#include "../inc/PiVisScheduler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define COLOR_IMAGE_SIZE (640 * 480 * 3)

static SchdRunFuncEntry funcEntry;

static unsigned char recBuffer[COLOR_IMAGE_SIZE];
static unsigned char prevBuffer[(COLOR_IMAGE_SIZE)];

static unsigned char receivedFirstImage = 0u;

static void run(void);

static void run(void)
{
	if((COLOR_IMAGE_SIZE) == CLIENT_receive((char*)recBuffer, (COLOR_IMAGE_SIZE)))
	{
		(void) printf("Finding diffs\n");
		unsigned int bufIndex = 0u;
		unsigned int colorIndex = 0u;
		unsigned int diff = 0u;
		unsigned int numDiffs = 0u;

		if(1u == receivedFirstImage)
		{
			for(; bufIndex < (COLOR_IMAGE_SIZE); ++bufIndex)
			{
				diff += abs(prevBuffer[bufIndex] - recBuffer[bufIndex]);

				colorIndex += 1u;

				if(colorIndex > 2)
				{
					if(diff > 100)
					{
						(void) printf("Found diff: %u. Buffers R: %u - %u, G: %u - %u, B: %u - %u\n",
								diff,
								prevBuffer[bufIndex - 2u],
								recBuffer[bufIndex - 2u],
								prevBuffer[bufIndex - 1u],
								recBuffer[bufIndex - 1u],
								prevBuffer[bufIndex],
								recBuffer[bufIndex]);
						numDiffs += 1u;
					}
					colorIndex = 0;
					diff = 0u;
				}
			}
		}

		receivedFirstImage = 1u;

		(void) memcpy(prevBuffer, recBuffer, (COLOR_IMAGE_SIZE));

		(void) printf("Num diffs found: %u\n", numDiffs);
	}
}


void IMAGEDIFF_init(void)
{
	funcEntry.run = run;
	funcEntry.next = 0;

	SCHED_registerCallback(&funcEntry);
}
