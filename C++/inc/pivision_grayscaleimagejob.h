#ifndef HEADER_PIVISION_GRAYSCALEIMAGEJOB
#define HEADER_PIVISION_GRAYSCALEIMAGEJOB

#include "pivision_events.h"
#include "jobbase.h"

class PiVisionGrayscaleImageJob : public JobBase
{
private:
  PiVisionGrayscaleImageJob();
  const std::shared_ptr<PiVisionImageData> colorImage;

protected:

public:
  PiVisionGrayscaleImageJob(const std::shared_ptr<PiVisionImageData> _colorImage);
  void Execute();
};

#endif
