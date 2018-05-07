#include "jobdispatcher.h"
#include "pivision_client.h"
#include "pivision_threadmodel.h"
#include "pivision_framecoord.h"
#include "pivision_events.h"
#include <memory>

int main(void)
{
  JobDispatcher::GetApi()->Log("PiVision start");

  PiVisionClient client;

  JobDispatcher::GetApi()->AddExecGroup(PIVISION_FRAMECOORD_THREAD_ID, 0u);

  std::shared_ptr<JobBase> frameCoordJob = std::make_shared<PiVisionFrameCoord>();

  JobDispatcher::GetApi()->ExecuteJobInGroup(frameCoordJob, PIVISION_FRAMECOORD_THREAD_ID);

  JobDispatcher::GetApi()->RaiseEvent(PIVISION_STOP, nullptr);

  JobDispatcher::DropInstance();

  return 0;
}
