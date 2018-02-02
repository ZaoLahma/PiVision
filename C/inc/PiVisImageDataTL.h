#ifndef HEADER_IMAGE_DATA_TL
#define HEADER_IMAGE_DATA_TL

void IMGDATATL_init(void);
void IMGDATATL_sendGrayscaleImage(unsigned char* buf,
		   	   	   	   	   	   	      unsigned int   size,
								                  unsigned short xSize,
							                	  unsigned short ySize);

unsigned int IMGDATATL_getNumDroppedFrames(void);

#endif
