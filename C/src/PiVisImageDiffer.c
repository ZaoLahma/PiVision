#include "../inc/PiVisImageDiffer.h"
#include "../inc/PiVisClient.h"
#include "../inc/PiVisScheduler.h"
#include "../inc/PiVisServer.h"
#include "../inc/PiVisConstants.h"
#include "../inc/PiVisImageProvider.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static SchdRunFuncEntry funcEntry;

static unsigned char recBuffer[(COLOR_IMAGE_SIZE)];
static unsigned char prevBuffer[(COLOR_IMAGE_SIZE)];

static unsigned char receivedFirstImage = 0u;

static PiVisServerContext serverContext;

static void run(void);

static void run(void)
{
	if((COLOR_IMAGE_SIZE) == IMGPROVIDER_getPixelData(recBuffer, (COLOR_IMAGE_SIZE)))
	{
		unsigned int bufIndex = 0u;
		unsigned int colorIndex = 0u;
		unsigned int diff = 0u;
		unsigned int numDiffs = 0u;

		char imageToSend[(GRAYSCALE_IMAGE_SIZE)] = "";
		unsigned int toSendPixelIndex = 0u;

		unsigned int colorIntensity = 0u;

		if(1u == receivedFirstImage)
		{
			for(; bufIndex < (COLOR_IMAGE_SIZE); ++bufIndex)
			{
				diff += abs(prevBuffer[bufIndex] - recBuffer[bufIndex]);

				colorIndex += 1u;
				colorIntensity += recBuffer[bufIndex];

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

		(void) memcpy(prevBuffer, recBuffer, (COLOR_IMAGE_SIZE));

		(void) printf("Num diffs found: %u\n", numDiffs);

		SERVER_send(&serverContext, imageToSend, (GRAYSCALE_IMAGE_SIZE));
	}
}


void IMAGEDIFF_init(void)
{
	serverContext.servedPortNo = (GRAY_PORT_NO);
	serverContext.serviceDiscoveryPortNo = (DISCOVER_GRAY_SERVICE);
	funcEntry.run = run;
	funcEntry.next = 0;

	SERVER_publishService(&serverContext);

	SCHED_registerCallback(&funcEntry);
}
