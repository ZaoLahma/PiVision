#include "pivision_client.h"
#include "jobdispatcher.h"

void PiVisionClient::HandleEvent(const uint32_t eventNo,
                                 std::shared_ptr<EventDataBase> dataPtr)
{
  switch(eventNo)
  {
    default:
      JobDispatcher::GetApi()->Log("PivisionClient received unexpected event: 0x%x", eventNo);
      break;
  }
}
