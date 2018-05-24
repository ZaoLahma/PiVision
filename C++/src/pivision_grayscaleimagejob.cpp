#include "pivision_grayscaleimagejob.h"
#include "jobdispatcher.h"
#include "pivision_services.h"

PiVisionGrayscaleImageJob::PiVisionGrayscaleImageJob(const std::shared_ptr<PiVisionImageData> _colorImage) :
colorImage(_colorImage)
{

}

void PiVisionGrayscaleImageJob::Execute()
{
  PiVisionDataBuf grayscaleImage;

  uint32_t xSize = colorImage->xSize;
  uint32_t ySize = colorImage->ySize;
  uint8_t colorIndex = 0u;
  uint16_t colorIntensity = 0u;

  for(uint32_t byteIndex = 0u; byteIndex < colorImage->imageData.size(); ++byteIndex)
  {
    colorIntensity += colorImage->imageData[byteIndex];
    colorIndex += 1u;

    if(colorIndex > 2u)
    {
      colorIndex = 0u;
      colorIntensity = colorIntensity / 3u;
      grayscaleImage.push_back(colorIntensity);
    }
  }

  auto newDataInd = std::make_shared<PiVisionNewDataInd>(grayscaleImage);
  JobDispatcher::GetApi()->RaiseEvent(PIVISION_BW_IMAGE_SERVICE_TX, newDataInd);
}
