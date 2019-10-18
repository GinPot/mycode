#ifndef _CAMERATESET_H_
#define _CAMERATESET_H_




typedef struct FbDevice {
	int g_fd;
	int xres;
	int yres;
	int bits_per_pixel;
	int iLineBytes;
	
	unsigned int g_dwScreenSize;
	unsigned char *g_pucFBMem;
}T_FbDevice;
 










#endif	/*_CAMERATESET_H_*/