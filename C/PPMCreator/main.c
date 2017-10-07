#include "PPMCreator.h"
#include <stdio.h>

#define TEST_IMAGE_FILE_NAME "test_image.ppm"
#define TEST_IMAGE_X_SIZE    (800u)
#define TEST_IMAGE_Y_SIZE    (600u)
#define TEST_IMAGE_BYTE_SIZE (TEST_IMAGE_X_SIZE * TEST_IMAGE_Y_SIZE)

static unsigned char testImageData[TEST_IMAGE_BYTE_SIZE];
static PPMCREATOR_imageData data;

int main(void)
{
  FILE* file = fopen(TEST_IMAGE_FILE_NAME, "wb");

  unsigned int testImageIndex = 0u;
  unsigned char pixelVal = 0u;
  for(; testImageIndex < TEST_IMAGE_BYTE_SIZE; ++testImageIndex)
  {
    testImageData[testImageIndex] = pixelVal;
    pixelVal += 1u;
  }

  PPMCREATOR_createGrayscaleImage(TEST_IMAGE_X_SIZE,
                                  TEST_IMAGE_Y_SIZE,
                                  testImageData,
                                  &data);

  (void) printf("%s", data.imageData);

  if(file)
  {
    fwrite(data.imageData, data.imageByteSize, 1, file);
  }

  fclose(file);

  return 0;
}
