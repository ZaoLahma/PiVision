#include "jobdispatcher.h"
#include "pivision_servicehandler.h"
#include "pivision_threadmodel.h"
#include "pivision_framecoord.h"
#include "pivision_ethterm.h"
#include "pivision_events.h"
#include <memory>

int main(void)
{
  JobDispatcher::GetApi()->Log("PiVision start");

  PiVisionEthTerm ethTerm;
  PiVisionServiceHandler services;

  JobDispatcher::GetApi()->AddExecGroup(PIVISION_FRAMECOORD_THREAD_ID, 0u);
  JobDispatcher::GetApi()->AddExecGroup(PIVISION_SERVICE_DISCOVERY_THREAD_ID, 5u);
  JobDispatcher::GetApi()->AddExecGroup(PIVISION_CONNECTIONS_THREAD_ID, 10u);

  PiVisionFrameCoord frameCoord;

  JobDispatcher::GetApi()->WaitForExecutionFinished();

  JobDispatcher::DropInstance();

  return 0;
}
