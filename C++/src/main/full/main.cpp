#include "jobdispatcher.h"
#include "pivision_servicehandler.h"
#include "pivision_threadmodel.h"
#include "pivision_framecoord.h"
#include "pivision_grayscaleimage.h"
#include "pivision_ethterm.h"
#include "pivision_events.h"
#include <memory>
#include <iostream>

int main(void)
{
  JobDispatcher::GetApi()->Log("PiVision start");

  JobDispatcher::GetApi()->AddExecGroup(EVENT_EXEC_GROUP_ID, 1u);
  JobDispatcher::GetApi()->AddExecGroup(PIVISION_FRAMECOORD_THREAD_ID, 0u);
  JobDispatcher::GetApi()->AddExecGroup(PIVISION_SERVICE_DISCOVERY_THREAD_ID, 5u);
  JobDispatcher::GetApi()->AddExecGroup(PIVISION_CONNECTIONS_THREAD_ID, 10u);
  JobDispatcher::GetApi()->AddExecGroup(PIVISION_SERVICE_LISTENER_THREAD_ID, 10u);
  JobDispatcher::GetApi()->AddExecGroup(PIVISION_GRAYSCALE_JOB_THREAD_ID, 1u);

  PiVisionEthTerm ethTerm;
  PiVisionServiceHandler services;
  PiVisionFrameCoord frameCoord;
  PiVisionGrayscaleImage grayScale;

  JobDispatcher::GetApi()->WaitForExecutionFinished();

  JobDispatcher::DropInstance();

  return 0;
}
