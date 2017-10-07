#include "PPMCreator.h"
#include <stdio.h>

#define PPM_IMAGE_HEADER_STRING  ("P6 %u %u %u\n")
#define PPM_IMAGE_HEADER_MAX_VAL (255u)

int PPMCREATOR_createGrayscaleImage(unsigned int xSize,
                                    unsigned int ySize,
                                    unsigned char* buf,
                                    PPMCREATOR_imageData* bufOut)
{
  int retVal = 0;

  bufOut->imageByteSize += sprintf((char*)bufOut->imageData,
                                   PPM_IMAGE_HEADER_STRING,
                                   xSize,
                                   ySize,
                                   PPM_IMAGE_HEADER_MAX_VAL);

  unsigned int bufIndex = 0u;

  for(; bufIndex < xSize * ySize; ++bufIndex)
  {
    bufOut->imageData[bufOut->imageByteSize] = buf[bufIndex];
    bufOut->imageByteSize += 1u;
    bufOut->imageData[bufOut->imageByteSize] = buf[bufIndex];
    bufOut->imageByteSize += 1u;
    bufOut->imageData[bufOut->imageByteSize] = buf[bufIndex];
    bufOut->imageByteSize += 1u;
  }

  return retVal;
}
