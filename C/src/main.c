#include "../inc/PiVisScheduler.h"
#include "../inc/PiVisClient.h"
#include "../inc/PiVisImageDiffer.h"

int main(void)
{
	SCHED_init();

	CLIENT_init();

	IMAGEDIFF_init();

	SCHED_run();

	return 0;
}
