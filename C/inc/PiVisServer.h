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
} PiVisServerContext;

void SERVER_init(void);
void SERVER_publishServiceContext(PiVisServerContext* context);
void SERVER_publishService(unsigned int portNo);
void SERVER_send(char* buf, unsigned int size);

#endif
