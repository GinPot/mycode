#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include <poll.h>
#include "CameraSo.h"



#define DevName "/dev/video0"
T_VideoDevice *ptVideoDevice;


static int *LutRv = NULL;
static int *LutGu = NULL;
static int *LutGv = NULL;
static int *LutBu = NULL;

unsigned char
R_FROMYV(unsigned char y, unsigned char v)
{
return CLIP((y) + LutRv[(v)]);
}
unsigned char
G_FROMYUV(unsigned char y, unsigned char u, unsigned char v)
{
return CLIP((y) + LutGu[(u)] + LutGv[(v)]);
}
unsigned char
B_FROMYU(unsigned char y, unsigned char u)
{
return CLIP((y) + LutBu[(u)]);
}

void initLut(void)
{
	int i;
	#define CoefRv 1402
	#define CoefGu 714 // 344
	#define CoefGv 344 // 714
	#define CoefBu 1772

	LutRv = malloc(256*sizeof(int));
	LutGu = malloc(256*sizeof(int));
	LutGv = malloc(256*sizeof(int));
	LutBu = malloc(256*sizeof(int));
	for (i= 0;i < 256;i++){

	    LutRv[i] = (i-128)*CoefRv/1000;
	    LutBu[i] = (i-128)*CoefBu/1000;
	    LutGu[i] = (128-i)*CoefGu/1000;
	    LutGv[i] = (128-i)*CoefGv/1000;
	}	
}
void freeLut(void){
	free(LutRv);
	free(LutGu);
	free(LutGv);
	free(LutBu);
}

int CameraInit(void)
{
	int fb, iError, i, ret;
	struct v4l2_capability tV4l2Cap;
	//struct v4l2_fmtdesc tFmtDesc;
	struct v4l2_format  tV4l2Fmt;
    struct v4l2_requestbuffers tV4l2ReqBuffs;
    struct v4l2_buffer tV4l2Buf;
	
	
	
	printf("%s enter\n",__func__);
	initLut();
	
	ptVideoDevice = malloc(sizeof(T_VideoDevice));
	if(!ptVideoDevice)
	{
    	printf("apply for ptVideoDevice mem fail\n");
        return -1;
	}
	memset(ptVideoDevice, 0, sizeof(T_VideoDevice));
	
	fb = open(DevName , O_RDWR);
	if(fb < 0)
	{
		printf("can not open %s\n" , DevName);
		goto errdmem_exit;
	}
	ptVideoDevice->I_fd = fb;
	
	memset(&tV4l2Cap, 0, sizeof(struct v4l2_capability));
	iError = ioctl(fb, VIDIOC_QUERYCAP, &tV4l2Cap);
    if (iError) 
	{
    	printf("Error opening device %s: unable to query device.\n", DevName);
    	goto errfb_exit;
    }

    if (!(tV4l2Cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
    	printf("%s is not a video capture device\n", DevName);
        goto errfb_exit;
    }

	if (tV4l2Cap.capabilities & V4L2_CAP_STREAMING) 
	    printf("%s supports streaming i/o\n", DevName);
	
    
	if (tV4l2Cap.capabilities & V4L2_CAP_READWRITE) 
	    printf("%s supports read i/o\n", DevName);
	
	/* set format in */
	memset(&tV4l2Fmt, 0, sizeof(struct v4l2_format));
    tV4l2Fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    tV4l2Fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    tV4l2Fmt.fmt.pix.width       = 720;
    tV4l2Fmt.fmt.pix.height      = 480;
    tV4l2Fmt.fmt.pix.field       = V4L2_FIELD_ANY;
    iError = ioctl(fb, VIDIOC_S_FMT, &tV4l2Fmt); 
    if (iError) 
    {
    	printf("Unable to set format\n");
        goto errfb_exit;        
    }
    ptVideoDevice->iWidth  = tV4l2Fmt.fmt.pix.width;
    ptVideoDevice->iHeight = tV4l2Fmt.fmt.pix.height;
	ptVideoDevice->iPixelFormat = V4L2_PIX_FMT_YUYV;
	ptVideoDevice->iLineBytes = ptVideoDevice->iWidth * 32 / 8;
	ptVideoDevice->iTotalBytes = ptVideoDevice->iLineBytes * ptVideoDevice->iHeight;
	ptVideoDevice->d_ConverMem = malloc(ptVideoDevice->iTotalBytes);
	if(!ptVideoDevice->d_ConverMem)
	{
    	printf("apply for conver mem fail\n");
        goto errfb_exit;
	}

	
    /* request buffers */
    memset(&tV4l2ReqBuffs, 0, sizeof(struct v4l2_requestbuffers));
    tV4l2ReqBuffs.count = NB_BUFFER;
    tV4l2ReqBuffs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    tV4l2ReqBuffs.memory = V4L2_MEMORY_MMAP;

    iError = ioctl(fb, VIDIOC_REQBUFS, &tV4l2ReqBuffs);
    if (iError) 
    {
    	printf("Unable to allocate buffers.\n");
        goto errmem_exit;        
    }
    
    ptVideoDevice->iVideoBufCnt = tV4l2ReqBuffs.count;
    if (tV4l2Cap.capabilities & V4L2_CAP_STREAMING)
    {
        /* map the buffers */
        for (i = 0; i < ptVideoDevice->iVideoBufCnt; i++) 
        {
        	memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
        	tV4l2Buf.index = i;
        	tV4l2Buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        	tV4l2Buf.memory = V4L2_MEMORY_MMAP;
        	iError = ioctl(fb, VIDIOC_QUERYBUF, &tV4l2Buf);
        	if (iError) 
            {
        	    printf("Unable to query buffer.\n");
        	    goto errmem_exit;
        	}

            ptVideoDevice->iVideoBufMaxLen = tV4l2Buf.length;
        	ptVideoDevice->pucVideBuf[i] = mmap(0 /* start anywhere */ ,
        			  tV4l2Buf.length, PROT_READ, MAP_SHARED, fb,
        			  tV4l2Buf.m.offset);
        	if (ptVideoDevice->pucVideBuf[i] == MAP_FAILED) 
            {
        	    printf("Unable to map buffer\n");
        	    goto errmem_exit;
        	}
        }

        /* Queue the buffers. */
        for (i = 0; i < ptVideoDevice->iVideoBufCnt; i++) 
        {
        	memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
        	tV4l2Buf.index = i;
        	tV4l2Buf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        	tV4l2Buf.memory = V4L2_MEMORY_MMAP;
        	iError = ioctl(fb, VIDIOC_QBUF, &tV4l2Buf);
        	if (iError)
            {
        	    printf("Unable to queue buffer.\n");
        	    goto errmem_exit;
        	}
        }
    }	

	ret = system("media-ctl -d /dev/media0 -V \"'rcar_csi2 feaa0000.csi2':1 [fmt:UYVY2X8/720x480 field:alternate]\"");
	ret = system("media-ctl -d /dev/media0 -V \"'adv748x 0-0070 afe':8 [fmt:UYVY2X8/720x480 field:alternate]\"");


	
	return 0;

	
errmem_exit:	
	free(ptVideoDevice->d_ConverMem);
errfb_exit:    
    close(fb);
errdmem_exit:
	free(ptVideoDevice);
    return -1;   	
}

/* translate YUV422Packed to rgb32 */
static unsigned int Pyuv422torgb32(unsigned char * input_ptr, unsigned char * output_ptr, unsigned int image_width, unsigned int image_height)
{
	unsigned int i, size;
	unsigned char Y, Y1, U, V;
	unsigned char *buff = input_ptr;
	unsigned int *output_pt = (unsigned int *)output_ptr;
    unsigned int r, g, b;
    unsigned int color;

	size = image_width * image_height /2;
	for (i = size; i > 0; i--) {
		/* bgr instead rgb ?? */
		Y = buff[0] ;
		U = buff[1] ;
		Y1 = buff[2];
		V = buff[3];
		buff += 4;

        r = R_FROMYV(Y,V);
		g = G_FROMYUV(Y,U,V); //b
		b = B_FROMYU(Y,U); //v
		/* rgb888 */
		color = (r << 16) | (g << 8) | b;
        *output_pt++ = color;
			
		r = R_FROMYV(Y1,V);
		g = G_FROMYUV(Y1,U,V); //b
		b = B_FROMYU(Y1,U); //v
		color = (r << 16) | (g << 8) | b;
        *output_pt++ = color;
	}
	
	return 0;
}

int PicMerge(int iX, int iY, unsigned char * ptSmallPic, unsigned char * ptBigPic)
{
	int i;
	unsigned char *pucSrc;
	unsigned char *pucDst;
	
	if ((ptVideoDevice->iWidth > ptVideoDevice->ptlFbDevice->xres)  ||
		(ptVideoDevice->iHeight > ptVideoDevice->ptlFbDevice->yres))
	{
		return -1;
	}

	pucSrc = ptSmallPic;
	pucDst = ptBigPic + iY * ptVideoDevice->ptlFbDevice->iLineBytes + iX * ptVideoDevice->ptlFbDevice->bits_per_pixel / 8;
	for (i = 0; i < ptVideoDevice->iHeight; i++)
	{
		memcpy(pucDst, pucSrc, ptVideoDevice->iLineBytes);
		pucSrc += ptVideoDevice->iLineBytes;
		pucDst += ptVideoDevice->ptlFbDevice->iLineBytes;
	}
	return 0;
}

static void * CameraCapturePthread(void *arg)
{
    struct pollfd tFds[1];
    int iRet, iTotalBytes;
    struct v4l2_buffer tV4l2Buf;
    int iTopLeftX;
    int iTopLeftY;
	
	while(1)
	{	
		/* poll */
		tFds[0].fd     = ptVideoDevice->I_fd;
		tFds[0].events = POLLIN;

		iRet = poll(tFds, 1, -1);
		if (iRet <= 0)
		{
			printf("poll error!\n");
			goto err_exit;
		}
		
		/* VIDIOC_DQBUF */
		memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
		tV4l2Buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		tV4l2Buf.memory = V4L2_MEMORY_MMAP;
		iRet = ioctl(ptVideoDevice->I_fd, VIDIOC_DQBUF, &tV4l2Buf);
		if (iRet < 0) 
		{
			printf("Unable to dequeue buffer.\n");
			goto err_exit;
		}	
		ptVideoDevice->iVideoBufCurIndex = tV4l2Buf.index;
		Pyuv422torgb32(ptVideoDevice->pucVideBuf[tV4l2Buf.index], ptVideoDevice->d_ConverMem, ptVideoDevice->iWidth, ptVideoDevice->iHeight);
		
        iTopLeftX = (ptVideoDevice->ptlFbDevice->xres - ptVideoDevice->iWidth) / 2;
        iTopLeftY = (ptVideoDevice->ptlFbDevice->yres - ptVideoDevice->iHeight) / 2;
		PicMerge(iTopLeftX, iTopLeftY, ptVideoDevice->d_ConverMem, ptVideoDevice->d_frameMem);
		
		memcpy(ptVideoDevice->ptlFbDevice->g_pucFBMem, ptVideoDevice->d_frameMem, ptVideoDevice->ptlFbDevice->g_dwScreenSize);
	
		memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
		tV4l2Buf.index  = ptVideoDevice->iVideoBufCurIndex;
		tV4l2Buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		tV4l2Buf.memory = V4L2_MEMORY_MMAP;
		iRet = ioctl(ptVideoDevice->I_fd, VIDIOC_QBUF, &tV4l2Buf);
		if (iRet) 
		{
			printf("Unable to queue buffer.\n");
			goto err_exit;
		}	
		
		pthread_testcancel();
		
	}
	
err_exit:  
	pthread_exit((void *)-1);  
}


int CameraCaptureStart(T_FbDevice *g_paraFBMem)
{
	int ret;
	int iType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	printf("%s enter\n",__func__);
	ptVideoDevice->ptlFbDevice = g_paraFBMem;
	
	//start camera capture
    ret = ioctl(ptVideoDevice->I_fd, VIDIOC_STREAMON, &iType);
    if (ret) 
    {
    	printf("Unable to start capture.\n");
    	goto err_exit;
    }

    /* creat pthread */
    if ((pthread_create(&ptVideoDevice->tidp, NULL, CameraCapturePthread, NULL)) == -1)
    {
        printf("create pthread error!\n");
        goto err_exit;
    }
	
	ptVideoDevice->d_frameMem = malloc(g_paraFBMem->g_dwScreenSize);
	if(!ptVideoDevice->d_ConverMem)
	{
    	printf("apply for framebuffer mem fail\n");
        goto err_exit;
	}
	return 0; 
	
err_exit:    
    return -1; 	
}

int CameraCaptureStop(void)
{
	int ret;
	printf("%s enter\n",__func__);
	ret = pthread_cancel(ptVideoDevice->tidp);
	if(ret != 0)
	{

		printf("pthread cancel fail!\n");
		goto err_exit;

	}

    /* 等待线程pthread释放 */
    if (pthread_join(ptVideoDevice->tidp, NULL))                  
    {
        printf("thread is not exit...\n");
        goto err_exit;
    }	
	printf("thread exit success.\n");
	free(ptVideoDevice->d_frameMem);
	
	memset(ptVideoDevice->ptlFbDevice->g_pucFBMem, 0, ptVideoDevice->ptlFbDevice->g_dwScreenSize);
	
	return 0;
	
err_exit:    
    return -1; 		
}


int CameraFree(void)
{
	int i;
	printf("%s enter\n",__func__);
    for (i = 0; i < ptVideoDevice->iVideoBufCnt; i++) 
		munmap(ptVideoDevice->pucVideBuf[i], ptVideoDevice->iVideoBufMaxLen);
	
	
	close(ptVideoDevice->I_fd);	
	free(ptVideoDevice->d_ConverMem);
	free(ptVideoDevice);
	
	freeLut();
	
	return 0;		
}
