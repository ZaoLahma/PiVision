#ifndef HEADER_PIVISION_GRAYSCALEIMAGEJOB
#define HEADER_PIVISION_GRAYSCALEIMAGEJOB

#include "pivision_events.h"
#include "jobbase.h"

class PiVisionGrayscaleImageJob : public JobBase
{
private:
  PiVisionGrayscaleImageJob();
  void ExtractImageProperties(uint16_t* xSize, uint16_t* ySize);
  const std::shared_ptr<PiVisionNewDataInd> colorImage;

protected:

public:
  PiVisionGrayscaleImageJob(const std::shared_ptr<PiVisionNewDataInd> _colorImage);
  void Execute();
};

#endif
