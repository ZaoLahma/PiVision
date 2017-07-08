#ifndef HEADER_PI_VIS_CLIENT
#define HEADER_PI_VIS_CLIENT

void CLIENT_init(void);

unsigned int CLIENT_receive(char* buf, unsigned int bufSize);
void CLIENT_send(char* buf, unsigned int bufSize);

#endif
