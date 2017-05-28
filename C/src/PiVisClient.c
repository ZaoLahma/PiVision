#include "../inc/PiVisClient.h"
#include "../inc/PiVisScheduler.h"
#include <stdio.h>

static SchdRunFuncEntry funcEntry;

static void run(void);

static void run(void)
{

}

void CLIENT_init(void)
{
	funcEntry.run = run;
	funcEntry.next = 0;

	SCHED_registerCallback(&funcEntry);
}
