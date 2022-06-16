#include "camera.h"


int InitUvcCamera(struct MainInfo *maininfo)
{
	int ret, i;
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	struct v4l2_capability cap;				//获取设备功能
	struct v4l2_fmtdesc fmt;				//获取设备支持格式
	struct v4l2_frmsizeenum frmsize;		//获取设备支持分辨率
	struct v4l2_format format;				//设置图像格式
	struct v4l2_streamparm streamparm;		//设置图像帧率
	struct v4l2_requestbuffers req_buff;	//设置图像采集缓冲区
	struct v4l2_buffer buff_info;			//获取缓冲区信息
	

	printf("\n********Init uvc camera********\n");
	
	maininfo->cameraFd = open(VIDEO_DEV, O_RDWR, 0);
	if(maininfo->cameraFd < 0){
		printf("open %s fail\n", VIDEO_DEV);
		return -1;
	}
	
/****************************************************************************************/

	if(ioctl(maininfo->cameraFd, VIDIOC_QUERYCAP, &cap) < 0){						//1.查看设备功能
		printf("requre VIDIOC_QUERYCAP fail\n");
		goto err_open;
	}
	//printf("\ndriver:\n    %s\n", cap.driver);
	printf("\ncard:\n    %s\n", cap.card);
	//printf("bus_info:\n    %s\n", cap.bus_info);
	//printf("version:\n    0x%x\n", cap.version);
	printf("\ncapabilities:\n    0x%x\n\n", cap.capabilities);

/****************************************************************************************/

	memset(&fmt, 0, sizeof(fmt));
	fmt.index = 0;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	printf("Support image format:\n");
	while(ioctl(maininfo->cameraFd, VIDIOC_ENUM_FMT, &fmt) == 0){					//2.获取camera图像格式
		printf("    %c%c%c%c -- %s\n", fmt.pixelformat & 0xff, (fmt.pixelformat >> 8) & 0xff,
									(fmt.pixelformat >> 16) & 0xff, (fmt.pixelformat >> 24) & 0xff,
									fmt.description);
		
		frmsize.pixel_format = fmt.pixelformat;
		frmsize.index = 0;
		while(ioctl(maininfo->cameraFd, VIDIOC_ENUM_FRAMESIZES, &frmsize) == 0){	//3.获取支持的分辨率
			if(frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
				printf("        %d: %dx%d\n",frmsize.type, frmsize.discrete.width, frmsize.discrete.height);

			frmsize.index++;
		}
		fmt.index++;
	}

/****************************************************************************************/

	memset(&format, 0, sizeof(struct v4l2_format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;							//4.设置视屏格式
	format.fmt.pix.width = IMAGE_WIDTH;
	format.fmt.pix.height = IMAGE_HEIGHT;
#if CAMERAMJPEG
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
#else 
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
#endif
	format.fmt.pix.field = V4L2_FIELD_ANY;								//系统自动设置: 帧属性		
	if(ioctl(maininfo->cameraFd, VIDIOC_S_FMT, &format) < 0){
		printf("set format fail\n");
		goto err_open;
	}
	if(ioctl(maininfo->cameraFd, VIDIOC_G_FMT, &format) < 0){
		printf("get format fail\n");
		goto err_open;
	}	
	printf("\nset format:\n");
	printf("    %c%c%c%c\n", format.fmt.pix.pixelformat & 0xff, (format.fmt.pix.pixelformat >> 8) & 0xff,
									(format.fmt.pix.pixelformat >> 16) & 0xff, (format.fmt.pix.pixelformat >> 24) & 0xff);
	printf("    %dx%d\n", format.fmt.pix.width, format.fmt.pix.height);
	printf("    field: %d\n", format.fmt.pix.field);
	printf("    bytesperline: %d\n", format.fmt.pix.bytesperline);

/****************************************************************************************/

	streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;						//5.设置帧率
	streamparm.parm.capture.timeperframe.numerator = 1;					//一秒
	streamparm.parm.capture.timeperframe.denominator = PERFRAME;		//25帧
	if(ioctl(maininfo->cameraFd, VIDIOC_S_PARM, &streamparm) < 0){
		printf("set stream parm fail\n");
		goto err_open;
	}
	if(ioctl(maininfo->cameraFd, VIDIOC_G_PARM, &streamparm) < 0){
		printf("get stream parm fail\n");
		goto err_open;
	}
	printf("\nFrames per second: %d/%d\n", streamparm.parm.capture.timeperframe.denominator, 
											streamparm.parm.capture.timeperframe.numerator);

/****************************************************************************************/

	memset(&req_buff, 0 , sizeof(struct v4l2_requestbuffers));
	req_buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req_buff.count = BUFFER_NUM;										//申请4个缓冲区
	req_buff.memory = V4L2_MEMORY_MMAP;									//支持mmap内存映射
//	req_buff.memory = V4L2_MEMORY_DMABUF;								//支持dmabuf内存映射
	if(ioctl(maininfo->cameraFd, VIDIOC_REQBUFS, &req_buff)){					//6.申请数据采集缓冲区
		printf("reques buffers fail\n");
		goto err_open;
	}

/****************************************************************************************/

	memset(&buff_info, 0, sizeof(struct v4l2_buffer));
	for(i = 0; i < req_buff.count; i++){
		buff_info.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buff_info.memory = V4L2_MEMORY_MMAP;
		buff_info.index = i;
		if(ioctl(maininfo->cameraFd, VIDIOC_QUERYBUF, &buff_info)){				//7.获取缓存区信息
			printf("get buffers fail\n");
			goto err_open;
		}
		//用获取的缓冲区信息: 用mmap将内核地址映射到用户空间
		maininfo->bufferInfo[i].length = buff_info.length;
		maininfo->bufferInfo[i].imageBuffer = mmap(NULL, buff_info.length, PROT_READ | PROT_WRITE, MAP_SHARED,
								maininfo->cameraFd, buff_info.m.offset);
		printf("bufferInfo[%d].imageBuffer=0x%p, length=%d\n", i, maininfo->bufferInfo[i].imageBuffer, maininfo->bufferInfo[i].length);
		if(maininfo->bufferInfo[i].imageBuffer == NULL){
			printf("mmap buffers fail\n");
			goto err_open;
		}
	}

/****************************************************************************************/

	memset(&buff_info, 0, sizeof(struct v4l2_buffer));
	for(i = 0; i < req_buff.count; i++){
		buff_info.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buff_info.memory = V4L2_MEMORY_MMAP;
//		buff_info.memory = V4L2_MEMORY_DMABUF;
		buff_info.index = i;
//		buff_info.m.fd = buf->dbuf_fd[i];
		if(ioctl(maininfo->cameraFd, VIDIOC_QBUF, &buff_info)){					//8.将缓冲区放入采集队列
			printf("set buffers fail\n");
			goto err_mmap;
		}
	}

/****************************************************************************************/

	if(ioctl(maininfo->cameraFd, VIDIOC_STREAMON, &type) < 0){					//启动数据采集
		printf("start stream fail\n");
		goto err_mmap;
	}


	return 0;

err_mmap:
	for(i = 0; i < req_buff.count; i++)
		munmap(maininfo->bufferInfo[i].imageBuffer, maininfo->bufferInfo[i].length);
err_open:
	close(maininfo->cameraFd);
	return -1;
}

void ExitUvcCamera(struct MainInfo *maininfo)
{
	int i;
	for(i = 0; i < BUFFER_NUM; i++)
		munmap(maininfo->bufferInfo[i].imageBuffer, maininfo->bufferInfo[i].length);
	close(maininfo->cameraFd);
}