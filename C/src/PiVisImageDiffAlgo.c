#include "PiVisImageDiffAlgo.h"
#include "PiVisAlgoCtrl.h"
#include "PiVisConstants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char receivedFirstImage = 0u;
static unsigned char prevBuffer[(COLOR_IMAGE_SIZE)];
static unsigned char imageToSend[(GRAYSCALE_IMAGE_SIZE)];

void* IMGDIFFALGO_exec(void* arg)
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
