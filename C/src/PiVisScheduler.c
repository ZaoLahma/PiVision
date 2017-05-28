#include <stdio.h>
#include "../inc/PiVisScheduler.h"

static SchdRunFuncEntry* registeredFuncs;
static int running;

void SCHED_init(void)
{
	registeredFuncs = 0;
	running = 0;
}

int SCHED_registerCallback(SchdRunFuncEntry* schdRunFuncEntry)
{
	schdRunFuncEntry->next = 0;

	SchdRunFuncEntry* lastEntry = registeredFuncs;

	if(0 == lastEntry)
	{
		registeredFuncs = schdRunFuncEntry;

		(void) printf("Registered first callback\n");
	}
	else
	{
		while(lastEntry->next != 0)
		{
			lastEntry = lastEntry->next;
		}

		lastEntry->next = schdRunFuncEntry;
	}

	return 0;
}

void SCHED_run(void)
{
	running = 1;

	while(1 == running)
	{
		SchdRunFuncEntry* toExecute = registeredFuncs;

		while(toExecute != 0)
		{
			toExecute->run();
			toExecute = toExecute->next;
		}
	}
}
