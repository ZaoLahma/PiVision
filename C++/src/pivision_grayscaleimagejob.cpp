#include "pivision_grayscaleimagejob.h"
#include "jobdispatcher.h"
#include "pivision_services.h"

PiVisionGrayscaleImageJob::PiVisionGrayscaleImageJob(const std::shared_ptr<PiVisionNewDataInd> _colorImage) :
colorImage(_colorImage)
{

}

#include <stdio.h>

void PiVisionGrayscaleImageJob::ExtractImageProperties(uint16_t* xSize, uint16_t* ySize)
{
  uint32_t shiftIndex = 0u;

  for(uint32_t i = 0u; i < sizeof(uint16_t); ++i)
  {
    *xSize = *xSize | (colorImage->dataBuf[i] << (shiftIndex * 8u));
    shiftIndex += 1u;
  }

  shiftIndex = 0u;

  for(uint32_t i = sizeof(uint16_t); i < 2 * sizeof(uint16_t); ++i)
  {
    *ySize = *ySize | (colorImage->dataBuf[i] << (shiftIndex * 8u));
    shiftIndex += 1u;
  }
}

void PiVisionGrayscaleImageJob::Execute()
{
  PiVisionDataBuf grayscaleImage;

  uint16_t extractedXSize = 0u;
  uint16_t extractedYSize = 0u;
  ExtractImageProperties(&extractedXSize, &extractedYSize);

  unsigned char xSize[sizeof(extractedXSize)];
  unsigned char ySize[sizeof(extractedYSize)];

  (void) memcpy(&xSize, &extractedXSize, sizeof(xSize));
  (void) memcpy(&ySize, &extractedYSize, sizeof(xSize));

  // grayscaleImage.push_back(xSize[0]);
  // grayscaleImage.push_back(xSize[1]);
  // grayscaleImage.push_back(ySize[0]);
  // grayscaleImage.push_back(ySize[1]);

  (void) printf("colorImage->dataBuf.size(): %u\n", colorImage->dataBuf.size());

  uint8_t colorIndex = 0u;
  uint16_t colorIntensity = 0u;

  for(uint32_t byteIndex = 4u; byteIndex < colorImage->dataBuf.size(); ++byteIndex)
  {
    colorIntensity += colorImage->dataBuf[byteIndex];
    colorIndex += 1u;

    if(colorIndex > 2u)
    {
      colorIndex = 0u;
      colorIntensity = colorIntensity / 3u;
      grayscaleImage.push_back(colorIntensity);
    }
  }

  auto newDataInd = std::make_shared<PiVisionNewDataInd>(grayscaleImage);
  (void) printf("grayscaleImage.size(): %u\n", grayscaleImage.size());
  JobDispatcher::GetApi()->RaiseEvent(PIVISION_BW_IMAGE_SERVICE_TX, newDataInd);
}
