#include "pivision_grayscaleimagejob.h"
#include "jobdispatcher.h"
#include "pivision_services.h"

#include <string.h>

PiVisionGrayscaleImageJob::PiVisionGrayscaleImageJob(const std::shared_ptr<PiVisionNewDataInd> _colorImage) :
colorImage(_colorImage)
{

}

void PiVisionGrayscaleImageJob::ExtractImageProperties(uint16_t* xSize, uint16_t* ySize)
{
  uint32_t shiftIndex = 0u;

  auto pixelData = colorImage->dataBuf;

  for(uint32_t i = 0u; i < sizeof(uint16_t); ++i)
  {
    *xSize = *xSize | ((*pixelData)[i] << (shiftIndex * 8u));
    shiftIndex += 1u;
  }

  shiftIndex = 0u;

  for(uint32_t i = sizeof(uint16_t); i < 2 * sizeof(uint16_t); ++i)
  {
    *ySize = *ySize | ((*pixelData)[i] << (shiftIndex * 8u));
    shiftIndex += 1u;
  }
}

void PiVisionGrayscaleImageJob::Execute()
{
  auto grayscaleImage = std::make_shared<PiVisionDataBuf>();

  uint16_t extractedXSize = 0u;
  uint16_t extractedYSize = 0u;
  ExtractImageProperties(&extractedXSize, &extractedYSize);

  unsigned char xSize[sizeof(extractedXSize)];
  unsigned char ySize[sizeof(extractedYSize)];

  (void) memcpy(&xSize, &extractedXSize, sizeof(xSize));
  (void) memcpy(&ySize, &extractedYSize, sizeof(xSize));

  uint8_t colorIndex = 0u;
  uint16_t colorIntensity = 0u;

  auto pixelData = colorImage->dataBuf;

  const uint32_t UPPER_LIMIT = 254u;
  const uint32_t LOWER_LIMIT = 1u;

  for(uint32_t byteIndex = 4u; byteIndex < pixelData->size(); ++byteIndex)
  {
    colorIntensity += (*pixelData)[byteIndex];
    colorIndex += 1u;

    if(colorIndex > 2u)
    {
      colorIndex = 0u;
      colorIntensity = colorIntensity / 3u;

      if(colorIntensity > UPPER_LIMIT)
      {
        colorIntensity = UPPER_LIMIT;
      }

      if(colorIntensity < LOWER_LIMIT)
      {
        colorIntensity = LOWER_LIMIT;
      }

      grayscaleImage->push_back(colorIntensity);
    }
  }

  auto newDataInd = std::make_shared<PiVisionNewDataInd>(PiVisionConnectionType::PIVISION_SERVER,
                                                         grayscaleImage);
  JobDispatcher::GetApi()->RaiseEvent(PIVISION_BW_IMAGE_SERVICE_TX, newDataInd);

  JobDispatcher::GetApi()->RaiseEvent(PIVISION_EVENT_GRAYSCALE_JOB_COMPLETE_IND, nullptr);
}
