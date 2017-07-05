#include "../inc/PiVisScheduler.h"
#include "../inc/PiVisClient.h"
#include "../inc/PiVisImageDiffer.h"
#include "../inc/PiVisServer.h"
#include "../inc/PiVisDiag.h"
#include "../inc/PiVisImageProvider.h"

int main(void)
{
	SCHED_init();

	CLIENT_init();

	SERVER_init();

	IMAGEDIFF_init();

	DIAG_init();

	//IMGPROVIDER_init();

	SCHED_run();

	return 0;
}
