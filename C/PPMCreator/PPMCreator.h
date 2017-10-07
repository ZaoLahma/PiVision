#ifndef HEADER_PPM_CREATOR
#define HEADER_PPM_CREATOR

#define PPM_IMAGE_MAX_BYTE_SIZE (2048 * 2048 * 3)

typedef struct
{
  unsigned int  imageByteSize;
  unsigned char imageData[PPM_IMAGE_MAX_BYTE_SIZE];
} PPMCREATOR_imageData;

int PPMCREATOR_createGrayscaleImage(unsigned int xSize,
                                    unsigned int ySize,
                                    unsigned char* rawPixelValues,
                                    PPMCREATOR_imageData* bufOut);

#endif
