#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <turbojpeg.h>
#include <linux/videodev2.h>

#include <vencoder.h>
#include <vdecoder.h>


#define VIDEO_DEV			"/dev/video0"
#define IMAGE_WIDTH			800
#define IMAGE_HEIGHT		600
#define PERFRAME			30
#define BUFFER_NUM			2
	
#define CAMERAMJPEG			1			//0: yuyv, 1: mjpeg
	
#define OUTFPS				1			//输出帧率
	
#define JPGFILE				0			//jpeg模式下保存为jpg文件
#define YUV420PFILE			0			//jpeg解码为yuv保存文件
#define YUV420PFILE_SOFT	0			//jpeg软件解码为yuv，并保存文件

#define H246FILE			0			//yuv编码为h24保存文件, NV21(YVU420SP)
	
	
	
	
#define YUVTOJPGFILE		0			//yuv软编码为jpeg文件



struct BufferInfo{
	unsigned char *imageBuffer;
	int length;
};


#define SHMKEY			52790				//共享内存key,用于互斥h264资源
#define ROI_NUM 		4
typedef struct {
    VencHeaderData          sps_pps_data;
    VencH264Param           h264Param;
    VencMBModeCtrl          h264MBMode;
    VencMBInfo              MBInfo;
    VencH264FixQP           fixQP;
    VencSuperFrameConfig    sSuperFrameCfg;
    VencH264SVCSkip         SVCSkip; // set SVC and skip_frame
    VencH264AspectRatio     sAspectRatio;
    VencH264VideoSignal     sVideoSignal;
    VencCyclicIntraRefresh  sIntraRefresh;
    VencROIConfig           sRoiConfig[ROI_NUM];
    VeProcSet               sVeProcInfo;
    VencOverlayInfoS        sOverlayInfo;
    VencSmartFun            sH264Smart;
}h264_func_t;

struct DmaInfo{
	char *physrc;
	int sizey;
	char *phydesty;
	char *phydestc;
};

struct MainInfo{
	//camera
	int width;
	int height;
	int cameraFd;
	struct BufferInfo bufferInfo[BUFFER_NUM];
	struct v4l2_buffer videoBuffer;

#if YUV420PFILE_SOFT
	//mjpeg2yuv420p
	tjhandle jpegDecompressor;
	tjhandle rgbCompressor;
	int rgbPixelFmt;
	int subSamp;
	int yuvSize;
	int rgbSize;
	unsigned char *rgbBuf;
	unsigned char *yuv420pbuf;
#endif
	
	//H264 encoder
#if H246FILE
	FILE *H264_file;
#endif
	int shmid;
	char *shm;							//分配的共享内存的原始首地址 
	VideoEncoder *pVideoEnc;
	h264_func_t h264_func;
	VencBaseConfig baseConfig;
	VencAllocateBufferParam bufferParam;

	//MJPEG decoder
#if YUV420PFILE
	FILE *yuv420p_file;
#endif
	char *mjpegBuffTmp;
	char *mjpegBuff;
	int mjpeBufLen;
	char *mjpegRingBuff;
	int mjpeRingBufLen;
	VideoDecoder *pVideo;
	VConfig vConfig;
	VideoStreamInfo videoInfo;
	VideoStreamDataInfo DataInfo;
	VideoPicture *videoPicture;

	//dma
	int dmafd;
	struct DmaInfo dmainfo;
};



#endif	/* __CONFIG_H__ */
