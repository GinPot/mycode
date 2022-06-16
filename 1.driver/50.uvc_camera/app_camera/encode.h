#ifndef _ENCODE_H
#define _ENCODE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <memoryAdapter.h>
#include <math.h>
#include <sys/shm.h>
#include <turbojpeg.h>

#include <dlfcn.h>
#include <vencoder.h>
#include <vdecoder.h>


#include "camera.h"
#include "rtmpsend264.h"
#include "ImageConfig.h" 

#define H264IMAGE_WIDTH		IMAGE_WIDTH
#define H264IMAGE_HEIGHT	(IMAGE_HEIGHT+8)
#define H264PERFRAME		PERFRAME


#define ALIGN_XXB(y, x) (((x) + ((y)-1)) & ~((y)-1))





typedef void VDPluginFun(void);

int InitEncode(struct MainInfo *maininfo);
void ExitEncode(struct MainInfo *maininfo);
int RunEncode(struct MainInfo *maininfo);

#if YUV420PFILE_SOFT
int MjpegToYuv422(struct MainInfo *maininfo, unsigned char* jpegBuffer, long unsigned int jpegSize);
int MjpegToYuv420p(struct MainInfo *maininfo, unsigned char* jpegBuffer, long unsigned int jpegSize);
int InitTurboJpeg(struct MainInfo *maininfo);
void ExitTurboJpeg(struct MainInfo *maininfo);
#endif

int InitMjpegDecode(struct MainInfo *maininfo);
int RunMjpegDecoder(struct MainInfo *maininfo);
void ExitMjpegDecoder(struct MainInfo *maininfo);


#endif	/* _ENCODE_H */