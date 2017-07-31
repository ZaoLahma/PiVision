#include "../inc/ThreadFramework/jobdispatcher.h"

int main(void)
{
	JobDispatcher::GetApi()->Log("PiVision starting...");
	JobDispatcher::GetApi()->DropInstance();
}
