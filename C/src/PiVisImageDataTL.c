#include "../inc/PiVisImageDataTL.h"
#include "../inc/PiVisServer.h"
#include "../inc/PiVisConstants.h"
#include "../inc/PiVisScheduler.h"

enum PiVisImageDataState
{
	SEND_DATA,
	WAIT_FOR_ACK
};

static SchdRunFuncEntry funcEntry;
static PiVisServerContext serverContext;

static void run(void);

static void run(void)
{
	//TODO: Wait for ACK from connected client and change state to "send"
}

void IMGDATATL_init(void)
{
	serverContext.servedPortNo = (GRAY_PORT_NO);
	serverContext.serviceDiscoveryPortNo = (DISCOVER_GRAY_SERVICE);

	SERVER_publishService(&serverContext);

	funcEntry.run = run;
	funcEntry.next = 0;

	SCHED_registerCallback(&funcEntry);
}

void IMGDATATL_send(char* buf, unsigned int size)
{
	//TODO: Fill header with relevant data before sending
	SERVER_send(&serverContext, buf, size);
}
