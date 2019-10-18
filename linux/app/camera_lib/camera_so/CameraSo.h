#ifndef _CAMERASO_H_
#define _CAMERASO_H_


#define NB_BUFFER 4


typedef struct FbDevice {
	int g_fd;
	int xres;
	int yres;
	int bits_per_pixel;
	int iLineBytes;
	
	unsigned int g_dwScreenSize;
	unsigned char *g_pucFBMem;
}T_FbDevice;

typedef struct VideoDevice	{
	int I_fd;
    int iPixelFormat;
    int iWidth;
    int iHeight;
	int iLineBytes;
	int iTotalBytes;
	
	pthread_t tidp;
	
    int iVideoBufCnt;
    int iVideoBufMaxLen;
    int iVideoBufCurIndex;
	unsigned char *d_ConverMem;
	unsigned char *d_frameMem;
	unsigned char *d_FbDeviceMem;
    unsigned char *pucVideBuf[NB_BUFFER];
	
	T_FbDevice *ptlFbDevice;
}T_VideoDevice;




#define CLIP(color) (unsigned char)(((color)>0xFF)?0xff:(((color)<0)?0:(color)))

unsigned char
R_FROMYV(unsigned char y, unsigned char v);

unsigned char
G_FROMYUV(unsigned char y, unsigned char u, unsigned char v);

unsigned char
B_FROMYU(unsigned char y, unsigned char u);

void initLut(void);
void freeLut(void);



#endif	/*_CAMERASO_H_*/