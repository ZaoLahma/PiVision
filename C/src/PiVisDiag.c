#include "PiVisDiag.h"
#include "PiVisScheduler.h"
#include "PiVisServer.h"
#include "PiVisImageDataTL.h"
#include <stdio.h>
#include <string.h>

#define DIAG_PORT_NO 3333
#define DIAG_SERVICE_DISCOVERY_PORT_NO 3334

static SchdRunFuncEntry funcEntry;
static PiVisServerContext serverContext;

static void run(void)
{
	if(serverContext.connected)
	{
		unsigned int droppedFrames = IMGDATATL_getNumDroppedFrames();
		char toSend[7] = "";
		sprintf(toSend, "%u\r\n", droppedFrames);
		SERVER_send(&serverContext, toSend, strlen(toSend));
	}
}

void DIAG_init(void)
{
	funcEntry.run = run;
	SCHED_registerCallback(&funcEntry);

	serverContext.servedPortNo = (DIAG_PORT_NO);
	serverContext.serviceDiscoveryPortNo = (DIAG_SERVICE_DISCOVERY_PORT_NO);

	SERVER_publishService(&serverContext);
}
