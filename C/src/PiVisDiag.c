#include "../inc/PiVisDiag.h"
#include "../inc/PiVisScheduler.h"
#include "../inc/PiVisServer.h"
#include "../inc/PiVisConstants.h"
#include <stdio.h>

#define DIAG_PORT_NO 3333
#define DIAG_SERVICE_DISCOVERY_PORT_NO 3334

static SchdRunFuncEntry funcEntry;
static PiVisServerContext serverContext;

static void run(void)
{
	if(serverContext.connected)
	{
		char toSend[] = "DIAG_DATA\n";
		SERVER_send(&serverContext, toSend, sizeof(toSend));
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
