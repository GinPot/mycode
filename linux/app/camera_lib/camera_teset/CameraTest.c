#include <unistd.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "CameraTest.h"

#define	FB_DEVICE_NAME	"/dev/fb0"


typedef int (*CameraCaptureStart_T)();
typedef int (*CameraCaptureStop_T)();
typedef int (*CameraInit_T)();
typedef int (*CameraFree_T)();

T_FbDevice *ptFbDevice;


static int FbInit(void)
{
	int ret;
	static struct fb_var_screeninfo g_tFBVar;
	static struct fb_fix_screeninfo g_tFBFix;	
	
	ptFbDevice = malloc(sizeof(T_FbDevice));
	if(!ptFbDevice)
	{
    	printf("apply for ptVideoDevice mem fail\n");
        return -1;
	}
	memset(ptFbDevice, 0, sizeof(T_FbDevice));
	
	
	ptFbDevice->g_fd = open(FB_DEVICE_NAME, O_RDWR);
	if (0 > ptFbDevice->g_fd)
	{
		printf("can't open %s\n", FB_DEVICE_NAME);
		goto errmem_exit;
	}	
	
	ret = ioctl(ptFbDevice->g_fd, FBIOGET_VSCREENINFO, &g_tFBVar);
	if (ret < 0)
	{
		printf("can't get fb's var\n");
		goto err_exit;
	}	
	
	ret = ioctl(ptFbDevice->g_fd, FBIOGET_FSCREENINFO, &g_tFBFix);
	if (ret < 0)
	{
		printf("can't get fb's fix\n");
		goto err_exit;
	}	
	
	ptFbDevice->xres			= g_tFBVar.xres;
	ptFbDevice->yres			= g_tFBVar.yres;
	ptFbDevice->bits_per_pixel	= g_tFBVar.bits_per_pixel;
	ptFbDevice->iLineBytes		= ptFbDevice->xres * g_tFBVar.bits_per_pixel / 8;
	
	ptFbDevice->g_dwScreenSize = ptFbDevice->iLineBytes * g_tFBVar.yres ;
	ptFbDevice->g_pucFBMem = (unsigned char *)mmap(NULL , ptFbDevice->g_dwScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, ptFbDevice->g_fd, 0);
	if (0 > ptFbDevice->g_pucFBMem)	
	{
		printf("can't mmap\n");
		goto err_exit;
	}
	
	printf("g_tFBVar.xres 				= %d\n", g_tFBVar.xres);
	printf("g_tFBVar.yres 				= %d\n", g_tFBVar.yres);
	printf("g_tFBVar.bits_per_pixel 	= %d\n", g_tFBVar.bits_per_pixel);
 
	
	
	return 0;
	
err_exit:
	free(ptFbDevice);
errmem_exit:
	close(ptFbDevice->g_fd);
    return -1; 	
}

static void FbFree(void)
{
	close(ptFbDevice->g_fd);
	munmap( ptFbDevice->g_pucFBMem, ptFbDevice->g_dwScreenSize);
}


int main(int argc, char **argv)
{
	int ret;
	char commd;
	
    void *handle=dlopen("./libcamer.so",RTLD_LAZY);	  //这里调用刚才生成的so库里的add方法
    
	CameraInit_T CameraInit_t=NULL;
	CameraFree_T CameraFree_t=NULL;
	CameraCaptureStop_T CameraCaptureStop_t=NULL;
	CameraCaptureStart_T CameraCaptureStart_t=NULL;
	
    *(void **)(&CameraInit_t)=dlsym(handle,"CameraInit");
	*(void **)(&CameraFree_t)=dlsym(handle,"CameraFree");
	*(void **)(&CameraCaptureStop_t)=dlsym(handle,"CameraCaptureStop");
    *(void **)(&CameraCaptureStart_t)=dlsym(handle,"CameraCaptureStart");
	
	ret = FbInit();
	if(ret < 0)
	{
		printf("FB Init fail!\n");
		return -1;
	}
	
	ret = CameraInit_t();
	if(ret < 0)
	{
		printf("Camera Init fail!\n");
		return -1;
	}
	ret = CameraFree_t();
	ret = CameraInit_t();
	if(ret < 0)
	{
		printf("Camera Init fail!\n");
		return -1;
	}
	
	while(1)
	{
		printf("Plense enter command:	");
		commd = getchar();
		
		if(commd == 'o')
		{	
			ret = CameraCaptureStart_t(ptFbDevice);
			if(ret < 0)
			{
				printf("camera capture start fail!\n");
				//return -1;
			}	
		}
		
		if(commd == 'f')
		{	
			ret = CameraCaptureStop_t();
			if(ret < 0)
			{
				printf("camera capture start fail!\n");
				//return -1;
			}	
		}	

		if(commd == 't')
		{	
			FbFree();
			ret = CameraFree_t();
			if(ret < 0)
			{
				printf("app free fail!\n");
				return -1;
			}
			return 0;
		}		
	}
	

    
}







