#include "PiVisImageNormalizerAlgo.h"
#include "PiVisAlgoCtrl.h"
#include "PiVisConstants.h"
#include <stdio.h>
#include <string.h>

#define U8_MAX (0xFFu)

static char grayscaleImage[(GRAYSCALE_IMAGE_SIZE)];
static char normalizedImage[(GRAYSCALE_IMAGE_SIZE)];
static unsigned int maxIntensity = 0u;
static unsigned int minIntensity = 255u;

static void createGrayscaleImage(char* colorImage);
static void normalizeImage(void);

static void createGrayscaleImage(char* colorImage)
{
  unsigned int colorIndex = 0u;
  unsigned int colorIntensity = 0u;
  unsigned int grayscalePixelIndex = 0u;

  unsigned int bufIndex = 0u;
  for(; bufIndex < (COLOR_IMAGE_SIZE); ++bufIndex)
  {
    colorIntensity += (colorImage[bufIndex] & U8_MAX);
    colorIndex += 1u;

    if(colorIndex > 2u)
    {
      colorIndex = 0u;
      colorIntensity = (colorIntensity / 3u);
      grayscaleImage[grayscalePixelIndex] = colorIntensity;

      if(colorIntensity > maxIntensity)
      {
        maxIntensity = colorIntensity;
      }

      if(colorIntensity < minIntensity)
      {
        minIntensity = colorIntensity;
      }

      colorIntensity = 0u;
      grayscalePixelIndex += 1u;
    }
  }
}

static void normalizeImage(void)
{
  unsigned int bufIndex = 0u;
  for(; bufIndex < (GRAYSCALE_IMAGE_SIZE); ++bufIndex)
  {

  }
}

void* IMGNORMALGO_exec(void* arg)
{
  (void) printf("Normalizing image\n");

  PiVisAlgoContext* algoContext = (PiVisAlgoContext*)(arg);
  algoContext->outputData = grayscaleImage;
  algoContext->outputDataSize = (GRAYSCALE_IMAGE_SIZE);

  createGrayscaleImage(algoContext->inputData);
  normalizeImage();

  maxIntensity = 0u;
  minIntensity = 255u;

  algoContext->state = ALGO_COMPLETE;

  return 0;
}
