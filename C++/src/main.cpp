#include "jobdispatcher.h"
#include "pivision_client.h"

int main(void)
{
  JobDispatcher::GetApi()->Log("PiVision start");

  PiVisionClient client;

  return 0;
}
