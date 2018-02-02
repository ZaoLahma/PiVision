#include "PiVisImageNormalizerAlgo.h"
#include "PiVisAlgoCtrl.h"
#include "PiVisConstants.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define U8_MAX (0xFFu)
#define IMAGE_FILTER_HIGH (240)
#define IMAGE_FILTER_LOW  (10)

static char grayscaleImage[(GRAYSCALE_IMAGE_SIZE)];
static char normalizedImage[(GRAYSCALE_IMAGE_SIZE)];
static int maxIntensity = 0u;
static int minIntensity = 255u;
static int averageIntensity = 0u;
static int standardDeviation = 0u;

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

      colorIntensity = 0u;
      grayscalePixelIndex += 1u;
    }
  }
}

static void normalizeImage(void)
{
  unsigned int bufIndex = 0u;
  int colorIntensity = 0u;
  for(; bufIndex < (GRAYSCALE_IMAGE_SIZE); ++bufIndex)
  {
    colorIntensity = grayscaleImage[bufIndex];

    if(colorIntensity > IMAGE_FILTER_HIGH)
    {
      colorIntensity = IMAGE_FILTER_HIGH;
    }
    else if(colorIntensity < IMAGE_FILTER_LOW)
    {
      colorIntensity = IMAGE_FILTER_LOW;
    }

    averageIntensity += colorIntensity;

    normalizedImage[bufIndex] = colorIntensity;
  }

  averageIntensity = averageIntensity / (GRAYSCALE_IMAGE_SIZE);
  unsigned int imageVariance = 0u;

  bufIndex = 0u;
  for(; bufIndex < (GRAYSCALE_IMAGE_SIZE); ++bufIndex)
  {
    imageVariance += (normalizedImage[bufIndex] - averageIntensity) * (grayscaleImage[bufIndex] - averageIntensity);
  }
  imageVariance = imageVariance / (GRAYSCALE_IMAGE_SIZE);

  standardDeviation = sqrt(imageVariance);

  (void) printf("averageIntensity: %u, standardDeviation: %u\n", averageIntensity, standardDeviation);

  bufIndex = 0u;
  for(; bufIndex < (GRAYSCALE_IMAGE_SIZE); ++bufIndex)
  {
    colorIntensity = normalizedImage[bufIndex];

    if(normalizedImage[bufIndex] > averageIntensity)
    {
      colorIntensity = (normalizedImage[bufIndex] - averageIntensity) / standardDeviation;
    }

    if(colorIntensity > maxIntensity)
    {
      maxIntensity = colorIntensity;
    }

    if(colorIntensity < minIntensity)
    {
      minIntensity = colorIntensity;
    }

    normalizedImage[bufIndex] = colorIntensity;
  }

  unsigned int normalizingFactor = (255 / (maxIntensity - minIntensity));
  bufIndex = 0u;
  for(; bufIndex < (GRAYSCALE_IMAGE_SIZE); ++bufIndex)
  {
    if(normalizedImage[bufIndex] > minIntensity)
    {
      normalizedImage[bufIndex] = (normalizedImage[bufIndex] - minIntensity) * normalizingFactor;
    }
  }
}

void* IMGNORMALGO_exec(void* arg)
{
  PiVisAlgoContext* algoContext = (PiVisAlgoContext*)(arg);

  createGrayscaleImage(algoContext->inputData);
  normalizeImage();

  algoContext->outputData = normalizedImage;
  algoContext->outputDataSize = (GRAYSCALE_IMAGE_SIZE);

  maxIntensity = 0u;
  minIntensity = 255u;

  algoContext->state = ALGO_COMPLETE;

  return 0;
}
