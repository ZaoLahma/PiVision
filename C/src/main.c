#include "../inc/PiVisScheduler.h"
#include "../inc/PiVisClient.h"

int main(void)
{
	SCHED_init();

	CLIENT_init();

	SCHED_run();

	return 0;
}
