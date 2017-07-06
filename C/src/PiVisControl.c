#include "../inc/PiVisControl.h"
#include "../inc/PiVisScheduler.h"
#include "../inc/PiVisServer.h"
#include <stdio.h>

#define CONTROL_PORT_NO (4444)
#define CONTROL_SERVICE_DISCOVERY_PORT_NO (4443)

#define COMMAND_BUF_LEN (256)

#define COMMAND_START_CAM 0x0
#define COMMAND_STOP_CAM  0x1

typedef struct CommandHeader_
{
	unsigned short command;
} CommandHeader;

static SchdRunFuncEntry funcEntry;
static PiVisServerContext serverContext;

static char commandBuf[(COMMAND_BUF_LEN)];

static void run(void);

static void run(void)
{
	int receivedBytes = SERVER_receive(&serverContext, commandBuf, COMMAND_BUF_LEN);

	if(-1 != receivedBytes)
	{
		CommandHeader* header = (CommandHeader*)commandBuf;

		(void) printf("Received data - size: %d, command: 0x%x\n", receivedBytes, header->command);

		switch(header->command)
		{
		case COMMAND_START_CAM:
			//Implement me
			break;
		case COMMAND_STOP_CAM:
			//Implement me
			break;
		default:
			break;
		}
	}
}

void CTRL_init(void)
{
	serverContext.servedPortNo = (CONTROL_PORT_NO);
	serverContext.serviceDiscoveryPortNo = (CONTROL_SERVICE_DISCOVERY_PORT_NO);
	funcEntry.run = run;
	funcEntry.next = 0;

	SERVER_publishService(&serverContext);

	SCHED_registerCallback(&funcEntry);
}
