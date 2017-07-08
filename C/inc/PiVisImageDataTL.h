#ifndef HEADER_IMAGE_DATA_TL
#define HEADER_IMAGE_DATA_TL

void IMGDATATL_init(void);
void IMGDATATL_send(char* buf, unsigned int size);
unsigned int IMGDATATL_getNumDroppedFrames(void);

#endif
