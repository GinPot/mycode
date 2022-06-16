#include "ImageConfig.h"
#include "camera.h"
#include "encode.h"
#include "rtmpsend264.h" 
#include "yuv_to_jpg.h"


#if OUTFPS
void OutPutFpsInfo()
{
	static int fps = 1; 
	struct timezone tz;
	static struct timeval tv_old, tv_new;
	float usec = 0, sec = 0, time = 0;

	if((fps%60) == 0){
		fps = 1;
		gettimeofday(&tv_old, &tz);
	}else{
		fps++;
		if(60 == fps){
			gettimeofday(&tv_new, &tz);
			if(tv_new.tv_usec >= tv_old.tv_usec){
				usec = tv_new.tv_usec - tv_old.tv_usec;
				sec = tv_new.tv_sec - tv_old.tv_sec;
			}else{
				usec = 1000000 + tv_new.tv_usec - tv_old.tv_usec;
				sec = tv_new.tv_sec - tv_old.tv_sec - 1;
			}
			time = sec + usec/1000000;
			printf("the fps is %f\n", 60/time);
		}
	}
}
#endif

static int QuitFlag = 0;
static void *ExitPthread(void *arg)
{
	char ch;

	while(1){
		ch = getchar();
		if(ch == 113){
			QuitFlag = 1;
			break;
		}
	}
	
	return 0;
}

int main(int argc, char **argv)
{
	int ret;
	struct MainInfo *maininfo;
	pthread_t tid_exit;

#if JPGFILE
	FILE *jpg_file;
#endif
#if YUV420PFILE_SOFT
	FILE *yuv_file;
	yuv_file=fopen("./3jpeg2yuvsoft.yuv", "wb");
#endif	
	
	if(pthread_create(&tid_exit, NULL, ExitPthread, NULL) != 0){
		printf("can't ExitPthread thread\n");
		return 0;
	}
	
	maininfo = malloc(sizeof(struct MainInfo));
	if(maininfo == NULL){
		printf("malloc struct MainInfo fail: %s.\n", strerror(errno));
		return 0;
	}
	memset(maininfo, 0, sizeof(struct MainInfo));

	if(InitUvcCamera(maininfo) < 0){
		printf("init uvc camera fail\n");
		return 0;
	}

#if YUV420PFILE_SOFT
	if(InitTurboJpeg(maininfo) < 0){
		printf("init TurboJpeg fail\n");
		return 0;
	}
#endif
	
	printf("\n");

	if(InitEncode(maininfo) < 0){
		printf("init H265 Encode fail\n");
		return 0;
	}
	if(InitMjpegDecode(maininfo) < 0){
		printf("init Mjpeg Decode fail\n");
		return 0;
	}
	RTMP264_Connect("rtmp://192.168.137.11:1935/live/111");



	while(1){
		memset(&maininfo->videoBuffer, 0, sizeof(struct v4l2_buffer));
		maininfo->videoBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		maininfo->videoBuffer.memory = V4L2_MEMORY_MMAP;
		if(ioctl(maininfo->cameraFd, VIDIOC_DQBUF, &maininfo->videoBuffer) < 0){		//v4l2:取出队列采集完毕的缓冲区
			printf("get camera buffer fail\n");
			return 0;
		}

#if JPGFILE
		jpg_file=fopen("./0out.jpg", "wb");
		fwrite(maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer, maininfo->bufferInfo[maininfo->videoBuffer.index].length, 1, jpg_file);
		fclose(jpg_file);
#endif
#if YUVTOJPGFILE
		Savejpegfile(maininfo);
#endif
#if YUV420PFILE_SOFT
		MjpegToYuv420p(maininfo, maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer, maininfo->bufferInfo[maininfo->videoBuffer.index].length);
		RunEncode(maininfo);
		fwrite(maininfo->yuv420pbuf, maininfo->yuvSize, 1, yuv_file);
#endif	




		// Mjpeg -> NV21(YVU420SP) -> H264 -> Rtmp
		if((RunMjpegDecoder(maininfo) < 0) || (QuitFlag == 1))
			break;

		//usleep(14*1000);





		if(ioctl(maininfo->cameraFd, VIDIOC_QBUF, &maininfo->videoBuffer) < 0){		//v4l2:将缓冲区再放入队列
			printf("set camera buffer fail\n");
			return 0;
		}
		
#if OUTFPS	
		OutPutFpsInfo();
#endif
	}
	
	pthread_join(tid_exit, (void**)&ret);

out:
#if YUV420PFILE_SOFT
	fclose(yuv_file);
#endif	
#if YUVTOJPGFILE
	ExitTurboJpeg(maininfo);
#endif
	ExitUvcCamera(maininfo);
	ExitEncode(maininfo);
	ExitMjpegDecoder(maininfo);
	free(maininfo);

	return 0;
}






