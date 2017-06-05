#include "../inc/PiVisImageDiffer.h"
#include "../inc/PiVisClient.h"
#include "../inc/PiVisScheduler.h"

#define COLOR_IMAGE_SIZE (640 * 480 * 3)

static SchdRunFuncEntry funcEntry;

static void run(void);

static void run(void)
{
	char buffer[COLOR_IMAGE_SIZE];

	CLIENT_receive(buffer, (COLOR_IMAGE_SIZE));
}


void IMAGEDIFF_init(void)
{
	funcEntry.run = run;
	funcEntry.next = 0;

	SCHED_registerCallback(&funcEntry);
}
