#ifndef HEADER_PI_VIS_SERVER
#define HEADER_PI_VIS_SERVER

typedef struct PiVisServerInternalContext_
{
	int serverSocket;
	int serviceDiscoverySocket;
	int clientSocket;
} PiVisServerInternalContext;

typedef struct PiVisServerContext_
{
	unsigned int servedPortNo;
	PiVisServerInternalContext serverContext;
	struct PiVisServerContext* next;
} PiVisServerContext;

void SERVER_init(void);
void SERVER_publishService(PiVisServerContext* context);
void SERVER_send(PiVisServerContext* context, char* buf, unsigned int size);

#endif
