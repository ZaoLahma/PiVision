#include "jobdispatcher.h"
#include "pivision_client.h"
#include "pivision_threadmodel.h"
#include "pivision_framecoord.h"
#include "pivision_ethterm.h"
#include "pivision_events.h"
#include <memory>

int main(void)
{
  JobDispatcher::GetApi()->Log("PiVision start");

  PiVisionEthTerm ethTerm;
  PiVisionClient client(3070);

  JobDispatcher::GetApi()->AddExecGroup(PIVISION_FRAMECOORD_THREAD_ID, 0u);
  JobDispatcher::GetApi()->AddExecGroup(PIVISION_SERVICE_DISCOVERY_THREAD_ID, 5u);
  JobDispatcher::GetApi()->AddExecGroup(PIVISION_CONNECTIONS_THREAD_ID, 10u);

  std::shared_ptr<JobBase> frameCoordJob = std::make_shared<PiVisionFrameCoord>();

  JobDispatcher::GetApi()->ExecuteJobInGroup(frameCoordJob, PIVISION_FRAMECOORD_THREAD_ID);

  JobDispatcher::GetApi()->WaitForExecutionFinished();

  JobDispatcher::DropInstance();

  return 0;
}
