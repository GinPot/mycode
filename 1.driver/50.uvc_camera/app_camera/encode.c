#include "encode.h"


/*********************************mjpeg/rgb//yuv*********************************/
#if YUV420PFILE_SOFT
// cup: 32%
void yuyv_to_yuv420P(unsigned char *in, unsigned char *out, int width, int height)  
{
	unsigned char *y, *u, *v;  
	int i, j, offset = 0, yoffset = 0;  
	 
	y = out;                                    //yuv420的y放在前面  
	u = out + (width * height);                 //yuv420的u放在y后  
	v = out + (width * height * 5 / 4);         //yuv420的v放在u后  
	//总共size = width * height * 3 / 2  
	 
	for(j = 0; j < height; j++)  
	{  
		yoffset = 2 * width * j;  
		for(i = 0; i < width * 2; i = i + 4)  
		{  
			offset = yoffset + i;  
			*(y ++) = *(in + offset);  
			*(y ++) = *(in + offset + 2);  
			if(j % 2 == 1)                      //抛弃奇数行的UV分量  
			{  
				*(u ++) = *(in + offset + 1);  
				*(v ++) = *(in + offset + 3);  
			}  
		}  
	}  
} 

// cup: 27%
int MjpegToYuv422(struct MainInfo *maininfo, unsigned char* jpegBuffer,long unsigned int jpegSize)
{
	//jpegToyuv422
	if (0 != tjDecompressToYUV2(maininfo->jpegDecompressor, jpegBuffer, jpegSize, maininfo->yuv420pbuf, maininfo->width, 4, maininfo->height, 0)) {
		printf("jpeg to yuv422 failure: %s\n", tjGetErrorStr());
		return -1;
	}

	return 0;
}


int MjpegToYuv420p(struct MainInfo *maininfo, unsigned char* jpegBuffer,long unsigned int jpegSize)
{
	//jpegTorgb		cup: 33%
	if (0 != tjDecompress2(maininfo->jpegDecompressor, jpegBuffer, jpegSize, maininfo->rgbBuf, maininfo->width, 0, maininfo->height, maininfo->rgbPixelFmt, 0)) {
		printf("jpeg to rgb failure: %s\n", tjGetErrorStr());
		return -1;
	}
	
	//rgbToyuv420p	cup: 10%
	if (0 != tjEncodeYUV3(maininfo->rgbCompressor, maininfo->rgbBuf, maininfo->width, 0, maininfo->height, maininfo->rgbPixelFmt, maininfo->yuv420pbuf, 4, maininfo->subSamp, 0)) {
		printf("rgb to yuv420p failure: %s\n", tjGetErrorStr());
		return -1;
	}
	
	return 0;
}

int InitTurboJpeg(struct MainInfo *maininfo)
{
	maininfo->jpegDecompressor 	= tjInitDecompress();
	maininfo->rgbCompressor 	= tjInitCompress();

	maininfo->width 			= IMAGE_WIDTH;
	maininfo->height 			= IMAGE_HEIGHT;
	maininfo->rgbPixelFmt 		= TJPF_RGB;
	maininfo->subSamp 			= TJSAMP_420;
	maininfo->rgbSize 			= maininfo->width * maininfo->height * 3;
	maininfo->yuvSize 			= maininfo->width * maininfo->height * 1.5;


    if((maininfo->rgbBuf = (unsigned char *)malloc(maininfo->rgbSize)) == NULL){
        printf("rgbBuf Memory allocation failure: %s\n", strerror(errno));
        return -1;
    }

    if((maininfo->yuv420pbuf = (unsigned char *)malloc(maininfo->yuvSize)) == NULL){
        printf("yuv420pbuf Memory allocation failure: %s\n", strerror(errno));
        goto yuv_err;
    }
	
	return 0;
	
yuv_err:
	free(maininfo->rgbBuf);
	return -1;
}

void ExitTurboJpeg(struct MainInfo *maininfo)
{
	free(maininfo->rgbBuf);
	free(maininfo->yuv420pbuf);

	tjDestroy(maininfo->jpegDecompressor);
	tjDestroy(maininfo->rgbCompressor);
}
#endif
/*********************************H264*********************************/

static long long GetNowUs()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000000 + now.tv_usec;
}

static void init_mb_mode(VencMBModeCtrl *pMBMode)
{
    unsigned int mb_num;
    unsigned int j;

    mb_num = (ALIGN_XXB(16, H264IMAGE_WIDTH) >> 4)
                * (ALIGN_XXB(16, H264IMAGE_HEIGHT) >> 4);
    pMBMode->p_info = malloc(sizeof(VencMBModeCtrlInfo) * mb_num);
    pMBMode->mode_ctrl_en = 1;

    for (j = 0; j < mb_num / 2; j++)
    {
        pMBMode->p_info[j].mb_en = 1;
        pMBMode->p_info[j].mb_skip_flag = 0;
        pMBMode->p_info[j].mb_qp = 22;
    }
    for (; j < mb_num; j++)
    {
        pMBMode->p_info[j].mb_en = 1;
        pMBMode->p_info[j].mb_skip_flag = 0;
        pMBMode->p_info[j].mb_qp = 32;
    }
}

static void init_mb_info(VencMBInfo *MBInfo)
{

    MBInfo->num_mb = (ALIGN_XXB(16, H264IMAGE_WIDTH) *
                            ALIGN_XXB(16, H264IMAGE_HEIGHT)) >> 8;

    MBInfo->p_para = (VencMBInfoPara *)malloc(sizeof(VencMBInfoPara) * MBInfo->num_mb);
    if(MBInfo->p_para == NULL)
    {
        printf("malloc MBInfo->p_para error\n");
        return;
    }
    printf("mb_num:%d, mb_info_queue_addr:%p\n", MBInfo->num_mb, MBInfo->p_para);
}

static void init_fix_qp(VencH264FixQP *fixQP)
{
    fixQP->bEnable = 1;
    fixQP->nIQp = 35;
    fixQP->nPQp = 35;
}

static void init_super_frame_cfg(VencSuperFrameConfig *sSuperFrameCfg)
{
    sSuperFrameCfg->eSuperFrameMode = VENC_SUPERFRAME_NONE;
    sSuperFrameCfg->nMaxIFrameBits = 30000*8;
    sSuperFrameCfg->nMaxPFrameBits = 15000*8;
}

static void init_svc_skip(VencH264SVCSkip *SVCSkip)
{
    SVCSkip->nTemporalSVC = T_LAYER_4;
    switch(SVCSkip->nTemporalSVC)
    {
        case T_LAYER_4:
            SVCSkip->nSkipFrame = SKIP_8;
            break;
        case T_LAYER_3:
            SVCSkip->nSkipFrame = SKIP_4;
            break;
        case T_LAYER_2:
            SVCSkip->nSkipFrame = SKIP_2;
            break;
        default:
            SVCSkip->nSkipFrame = NO_SKIP;
            break;
    }
}

static void init_aspect_ratio(VencH264AspectRatio *sAspectRatio)
{
    sAspectRatio->aspect_ratio_idc = 255;
    sAspectRatio->sar_width = 4;
    sAspectRatio->sar_height = 3;
}

static void init_video_signal(VencH264VideoSignal *sVideoSignal)
{
    sVideoSignal->video_format = 5;
    sVideoSignal->src_colour_primaries = 5;
    sVideoSignal->dst_colour_primaries = 8;
}

static void init_intra_refresh(VencCyclicIntraRefresh *sIntraRefresh)
{
    sIntraRefresh->bEnable = 1;
    sIntraRefresh->nBlockNumber = 10;
}

static void init_roi(VencROIConfig *sRoiConfig)
{
    sRoiConfig[0].bEnable = 1;
    sRoiConfig[0].index = 0;
    sRoiConfig[0].nQPoffset = 10;
    sRoiConfig[0].sRect.nLeft = 0;
    sRoiConfig[0].sRect.nTop = 0;
    sRoiConfig[0].sRect.nWidth = 352;
    sRoiConfig[0].sRect.nHeight = 288;

    sRoiConfig[1].bEnable = 1;
    sRoiConfig[1].index = 1;
    sRoiConfig[1].nQPoffset = 10;
    sRoiConfig[1].sRect.nLeft = 320;
    sRoiConfig[1].sRect.nTop = 180;
    sRoiConfig[1].sRect.nWidth = 320;
    sRoiConfig[1].sRect.nHeight = 180;

    sRoiConfig[2].bEnable = 1;
    sRoiConfig[2].index = 2;
    sRoiConfig[2].nQPoffset = 10;
    sRoiConfig[2].sRect.nLeft = 320;
    sRoiConfig[2].sRect.nTop = 180;
    sRoiConfig[2].sRect.nWidth = 320;
    sRoiConfig[2].sRect.nHeight = 180;

    sRoiConfig[3].bEnable = 1;
    sRoiConfig[3].index = 3;
    sRoiConfig[3].nQPoffset = 10;
    sRoiConfig[3].sRect.nLeft = 320;
    sRoiConfig[3].sRect.nTop = 180;
    sRoiConfig[3].sRect.nWidth = 320;
    sRoiConfig[3].sRect.nHeight = 180;
}



static void init_enc_proc_info(VeProcSet *ve_proc_set)
{
    ve_proc_set->bProcEnable = 1;
    ve_proc_set->nProcFreq = 3;
}


int InitEncode(struct MainInfo *maininfo)
{
	unsigned int vbv_size = 20*1024*1024, i;
	VencHeaderData sps_pps_data;

#if H246FILE	
	maininfo->H264_file = fopen("./4yuv2h264.h264", "wb");
#endif

    //create encoder
    maininfo->pVideoEnc = VideoEncCreate(VENC_CODEC_H264);
	
	/******** init h264 param ********/
    maininfo->h264_func.h264Param.bEntropyCodingCABAC = 1;
    maininfo->h264_func.h264Param.nBitrate = 20 * H264IMAGE_WIDTH * H264IMAGE_HEIGHT;
    maininfo->h264_func.h264Param.nFramerate = H264PERFRAME;
    maininfo->h264_func.h264Param.nCodingMode = VENC_FRAME_CODING;
    maininfo->h264_func.h264Param.nMaxKeyInterval = H264PERFRAME;
    maininfo->h264_func.h264Param.sProfileLevel.nProfile = VENC_H264ProfileHigh;
    maininfo->h264_func.h264Param.sProfileLevel.nLevel = VENC_H264Level51;
    maininfo->h264_func.h264Param.sQPRange.nMinqp = 33;//33;
    maininfo->h264_func.h264Param.sQPRange.nMaxqp = 50;
    maininfo->h264_func.h264Param.bLongRefEnable = 1;
    maininfo->h264_func.h264Param.nLongRefPoc = 0;
	
    //maininfo->h264_func.sH264Smart.img_bin_en = 1;
    //maininfo->h264_func.sH264Smart.img_bin_th = 27;
    //maininfo->h264_func.sH264Smart.shift_bits = 2;
    //maininfo->h264_func.sH264Smart.smart_fun_en = 1;

    //init VencMBModeCtrl
    //init_mb_mode(&maininfo->h264_func.h264MBMode);
    //init VencMBInfo
    //init_mb_info(&maininfo->h264_func.MBInfo);
    //init VencH264FixQP
    //init_fix_qp(&maininfo->h264_func.fixQP);
    //init VencSuperFrameConfig
    //init_super_frame_cfg(&maininfo->h264_func.sSuperFrameCfg);
    //init VencH264SVCSkip
    //init_svc_skip(&maininfo->h264_func.SVCSkip);
    //init VencH264AspectRatio
    //init_aspect_ratio(&maininfo->h264_func.sAspectRatio);
    //init VencH264AspectRatio
    init_video_signal(&maininfo->h264_func.sVideoSignal);
    //init CyclicIntraRefresh
    //init_intra_refresh(&maininfo->h264_func.sIntraRefresh);
    //init VencROIConfig
    //init_roi(maininfo->h264_func.sRoiConfig);
    //init proc info
    //init_enc_proc_info(&maininfo->h264_func.sVeProcInfo);
    //init VencOverlayConfig
    //init_overlay_info(&maininfo->h264_func.sOverlayInfo);

    VideoEncSetParameter(maininfo->pVideoEnc, VENC_IndexParamH264Param, &maininfo->h264_func.h264Param);
    VideoEncSetParameter(maininfo->pVideoEnc, VENC_IndexParamSetVbvSize, &vbv_size);
	VideoEncSetParameter(maininfo->pVideoEnc, VENC_IndexParamH264VideoSignal, &maininfo->h264_func.sVideoSignal);
	/******** end init h264 param ********/
	
	
	/******** begin set baseConfig param ********/
    maininfo->baseConfig.memops = MemAdapterGetOpsS();
    if (maininfo->baseConfig.memops == NULL)
    {
        printf("%s: MemAdapterGetOpsS failed\n", __func__);
        return -1;
    }
    CdcMemOpen(maininfo->baseConfig.memops);
    maininfo->baseConfig.nInputWidth= H264IMAGE_WIDTH;
    maininfo->baseConfig.nInputHeight = H264IMAGE_HEIGHT;
    maininfo->baseConfig.nStride = H264IMAGE_WIDTH;
    maininfo->baseConfig.nDstWidth = H264IMAGE_WIDTH;
    maininfo->baseConfig.nDstHeight = H264IMAGE_HEIGHT;

    maininfo->baseConfig.eInputFormat = VENC_PIXEL_YVU420SP;
	
	VideoEncInit(maininfo->pVideoEnc, &maininfo->baseConfig);
	/******** end set baseConfig param********/

    maininfo->bufferParam.nSizeY = H264IMAGE_WIDTH * H264IMAGE_HEIGHT;
    maininfo->bufferParam.nSizeC = H264IMAGE_WIDTH * H264IMAGE_HEIGHT / 2;
    maininfo->bufferParam.nBufferNum = 1;
	AllocInputBuffer(maininfo->pVideoEnc, &maininfo->bufferParam);
	
	
	
	VideoEncGetParameter(maininfo->pVideoEnc, VENC_IndexParamH264SPSPPS, &sps_pps_data);
#if H246FILE
    fwrite(sps_pps_data.pBuffer, 1, sps_pps_data.nLength, maininfo->H264_file);
#endif


	if(init_sps_pps_data(sps_pps_data.pBuffer, sps_pps_data.nLength) < 0){
		printf("init_sps_pps_data fail\n");
		goto memerror;
	}
		

	
	maininfo->shmid = shmget((key_t)SHMKEY, 1, 0666|IPC_CREAT);					//创建共享内存
    if(maininfo->shmid == -1)
    {  
        printf("shmget failed\n");
        goto memerror;
    }
	maininfo->shm = (char *)shmat(maininfo->shmid, (void*)0, 0);				//将共享内存连接到当前进程的地址空间
	*maininfo->shm = 0;
	printf("shm=%d\n", *maininfo->shm);
	
	maininfo->dmafd = open("/dev/dmadriver", O_RDWR, 0);
	if(maininfo->dmafd < 0){
		printf("open /dev/dmadriver fail\n");
		goto memerror;
	}

	return 0;
	
memerror:
	VideoEncDestroy(maininfo->pVideoEnc);
	CdcMemClose(maininfo->baseConfig.memops);
	return -1;
}


void ExitEncode(struct MainInfo *maininfo)
{
#if H246FILE
	fclose(maininfo->H264_file);
#endif
	VideoEncDestroy(maininfo->pVideoEnc);
	CdcMemClose(maininfo->baseConfig.memops);
	close(maininfo->dmafd);
}


int RunEncode(struct MainInfo *maininfo)
{
	int i;
	static long long pts = 0;
	static long long time1=0;
	static long long time2=0;
	VencInputBuffer inputBuffer;
	VencOutputBuffer outputBuffer;

    int nSizeY;
    nSizeY = maininfo->videoPicture->nWidth * maininfo->videoPicture->nHeight;

	GetOneAllocInputBuffer(maininfo->pVideoEnc, &inputBuffer);

//	memcpy(inputBuffer.pAddrVirY, maininfo->yuv420pbuf, H264IMAGE_WIDTH * H264IMAGE_HEIGHT);
//	memcpy(inputBuffer.pAddrVirC, maininfo->yuv420pbuf + H264IMAGE_WIDTH * H264IMAGE_HEIGHT, H264IMAGE_WIDTH * H264IMAGE_HEIGHT / 2);
	
	
	

	//memcpy(inputBuffer.pAddrVirY, maininfo->videoPicture->pData0, nSizeY);
	//memcpy(inputBuffer.pAddrVirC, maininfo->videoPicture->pData0 + nSizeY, nSizeY/2);

	maininfo->dmainfo.physrc = (char *)CdcMemGetPhysicAddressCpu(maininfo->vConfig.memops, maininfo->videoPicture->pData0);;
	maininfo->dmainfo.sizey = nSizeY;
	maininfo->dmainfo.phydesty = inputBuffer.pAddrPhyY;
	maininfo->dmainfo.phydestc = inputBuffer.pAddrPhyC;

	write(maininfo->dmafd, &maininfo->dmainfo, sizeof(struct DmaInfo));
	//printf("               physrc=%p, sizey=%d, phydesty=%p, phydestc=%p\n", maininfo->dmainfo.physrc, maininfo->dmainfo.sizey, maininfo->dmainfo.phydesty, maininfo->dmainfo.phydestc);
	
	

	//printf("\n               ");
	//for(i=0; i < 50; i++)
	//	printf("0x%2x ", inputBuffer.pAddrVirC[i]);
	//printf("\n");




	
    inputBuffer.bEnableCorp = 0;
    inputBuffer.sCropInfo.nLeft =  240;
    inputBuffer.sCropInfo.nTop  =  240;
    inputBuffer.sCropInfo.nWidth  =  240;
    inputBuffer.sCropInfo.nHeight =  240;
    FlushCacheAllocInputBuffer(maininfo->pVideoEnc, &inputBuffer);
    pts += 1*1000/H264PERFRAME;
    inputBuffer.nPts = pts;
    AddOneInputBuffer(maininfo->pVideoEnc, &inputBuffer);
	
    //time1 = GetNowUs();
	while(*maininfo->shm != 0){
		usleep(100);
	}
	*maininfo->shm = 1;
    VideoEncodeOneFrame(maininfo->pVideoEnc);
	*maininfo->shm = 0;
    //time2 = GetNowUs();
    //logv("encode frame %d use time is %lldus..\n",testNumber,(time2-time1));

    AlreadyUsedInputBuffer(maininfo->pVideoEnc,&inputBuffer);
    ReturnOneAllocInputBuffer(maininfo->pVideoEnc, &inputBuffer);

    if(GetOneBitstreamFrame(maininfo->pVideoEnc, &outputBuffer) == -1)
    {
		printf("GetOneBitstreamFrame fail\n");
        return -1;
    }


	
	//printf("outputBuffer.nSize0: %d\n", outputBuffer.nSize0);
	//for(i = 0; i < outputBuffer.nSize0; i++)
	//	printf("0x%x ", *(outputBuffer.pData0 + i));
	//printf("\n");
	
	send_rtmp_h264(outputBuffer.pData0, outputBuffer.nSize0);

#if H246FILE	
    fwrite(outputBuffer.pData0, 1, outputBuffer.nSize0, maininfo->H264_file);	
    if(outputBuffer.nSize1)
    {
       fwrite(outputBuffer.pData1, 1, outputBuffer.nSize1, maininfo->H264_file);
    }
#endif

    FreeOneBitStreamFrame(maininfo->pVideoEnc, &outputBuffer);

	
	return 0;
}

/*********************************MJPEG Decoder*********************************/


static void InitVDLib(const char *lib)
{
    void *libFd = NULL;
    if(lib == NULL)
    {
        printf("%s: open lib == NULL\n", __func__);
        return;
    }

    libFd = dlopen(lib, RTLD_NOW);

    VDPluginFun *PluginInit = NULL;

    if (libFd == NULL)
    {
        printf("%s: dlopen '%s' fail: %s\n", __func__, lib, dlerror());
        return ;
    }

    PluginInit = (VDPluginFun*)dlsym(libFd, "CedarPluginVDInit");
    if (PluginInit == NULL)
    {
        printf("%s: Invalid plugin, CedarPluginVDInit not found, lib:%s.\n", __func__, lib);
        return;
    }
    printf("%s: vdecoder open lib: %s\n", __func__, lib);
    PluginInit(); /* init plugin */
    return ;
}

static void AddVDLib(void)
{
//    InitVDLib("libawh265.so");
//    InitVDLib("libawh265soft.so");
//    InitVDLib("libawh264.so");
    InitVDLib("libawmjpeg.so");
//    InitVDLib("libawmjpegplus.so");
//    InitVDLib("libawmpeg2.so");
//    InitVDLib("libawmpeg4base.so");
//    InitVDLib("libawmpeg4normal.so");
//    InitVDLib("libawmpeg4vp6.so");
//    InitVDLib("libawmpeg4dx.so");
//    InitVDLib("libawmpeg4h263.so");
//    InitVDLib("libawvp8.so");
//    InitVDLib("libawwmv3.so");
//    InitVDLib("libawvp9soft.so");
//    InitVDLib("libawvp9Hw.so");

    return;
}

int InitMjpegDecode(struct MainInfo *maininfo)
{
	char fixdata[] = {0xFF, 0xD8, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x02, 0x58, 0x03, 0x20, 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF, 0xDB , 0x00, 0x43};


#if YUV420PFILE
	maininfo->yuv420p_file = fopen("./1jpeg2yuv.yuv", "wb");
#endif
	
	AddVDLib();

    maininfo->pVideo = CreateVideoDecoder();

    if(!maininfo->pVideo){
        printf("%s: create video decoder failed\n", __func__);
        return -1;
    }
	printf("%s: create video decoder ok\n", __func__);


    maininfo->vConfig.bGpuBufValid = 0;
    maininfo->vConfig.nAlignStride = 16;
    maininfo->vConfig.nDecodeSmoothFrameBufferNum = 3;
	maininfo->vConfig.eOutputPixelFormat = PIXEL_FORMAT_NV21;
	maininfo->vConfig.memops = MemAdapterGetOpsS();
    if(maininfo->vConfig.memops == NULL){
        printf("%s: MemAdapterGetOpsS failed\n", __func__);
        goto err_video;
    }
	CdcMemOpen(maininfo->vConfig.memops);
	
	maininfo->videoInfo.eCodecFormat = VIDEO_CODEC_FORMAT_MJPEG;
    maininfo->videoInfo.nWidth  = IMAGE_WIDTH;
    maininfo->videoInfo.nHeight = IMAGE_HEIGHT;
    maininfo->videoInfo.nFrameRate = H264PERFRAME;
	
    if((InitializeVideoDecoder(maininfo->pVideo, &maininfo->videoInfo, &maininfo->vConfig)) != 0){
        printf("%s: init decode error !\n", __func__);
        goto err_memops;
    }
    printf("%s: Initialize mjpeg decoder ok\n", __func__);

    maininfo->DataInfo.nStreamIndex = 0;
    maininfo->DataInfo.bIsFirstPart = 1;
    maininfo->DataInfo.bIsLastPart  = 1;
    maininfo->DataInfo.bValid       = 1;

	
	
	
	maininfo->mjpegBuffTmp = malloc(maininfo->bufferInfo[maininfo->videoBuffer.index].length);
	if(maininfo->mjpegBuffTmp == NULL){
		printf("malloc mjpegBuffTmp fail: %s.\n", strerror(errno));
		goto err_memops;
	}
	memcpy(maininfo->mjpegBuffTmp, fixdata, sizeof(fixdata));
	maininfo->mjpegBuffTmp[90] = 0xFF;
	maininfo->mjpegBuffTmp[91] = 0xDB;
	maininfo->mjpegBuffTmp[92] = 0x00;
	maininfo->mjpegBuffTmp[93] = 0x43;
	
	return 0;


err_memops:
	CdcMemClose(maininfo->vConfig.memops);
err_video:
	DestroyVideoDecoder(maininfo->pVideo);
}

#if YUV420PFILE
//YVU420SP
static void savePicture(struct MainInfo *maininfo)
{
    int nSizeY, nSizeUV, nSize;
    char *pData = NULL;

    nSizeY = maininfo->videoPicture->nWidth * maininfo->videoPicture->nHeight;
//    nSizeUV = nSizeY >> 2;
	nSizeUV = nSizeY >> 1;
    printf("%s: save picture , size: %dx%d, top: %d, bottom: %d, left: %d, right: %d\n",
            __func__, maininfo->videoPicture->nWidth, maininfo->videoPicture->nHeight, maininfo->videoPicture->nTopOffset,
            maininfo->videoPicture->nBottomOffset, maininfo->videoPicture->nLeftOffset, maininfo->videoPicture->nRightOffset);
			
//    int nSaveLen;
//    if( maininfo->videoPicture->bEnableAfbcFlag)
//         nSaveLen =  maininfo->videoPicture->nAfbcSize;
//    else
//         nSaveLen =  maininfo->videoPicture->nWidth *  maininfo->videoPicture->nHeight * 3/2;
//
//
//    CdcMemFlushCache(maininfo->vConfig.memops, maininfo->videoPicture->pData0, nSaveLen);
			
    /* save y */
    pData = maininfo->videoPicture->pData0;
    nSize = nSizeY;
    fwrite(pData, 1, nSize, maininfo->yuv420p_file);

    /* save u */
//    pData = maininfo->videoPicture->pData0 + nSizeY + nSizeUV;
//    nSize = nSizeUV;
//    fwrite(pData, 1, nSize, maininfo->yuv420p_file);
//
//    /* save v */
//    pData = maininfo->videoPicture->pData0 + nSizeY;
//    nSize = nSizeUV;
//    fwrite(pData, 1, nSize, maininfo->yuv420p_file);

    /* save vu */
    pData = maininfo->videoPicture->pData0 + nSizeY;
    nSize = nSizeUV;
    fwrite(pData, 1, nSize, maininfo->yuv420p_file);
}
#endif

static int GetExifLength(unsigned char *buff, struct MainInfo *maininfo)
{
	int length;// = 40000;
	unsigned char *tmbuff = buff;
	static int prelength = 50000;
	
	
	
	for(length = prelength; length <  maininfo->bufferInfo[maininfo->videoBuffer.index].length; length++){
		if(( maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer[length] == 0xFF) && ( maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer[length+1] == 0xD9)){
			length = length + 1;
			prelength = length - 1500;
			break;
		}
	}
	
	//length = 0;
	
//	printf("%s: prelength=%d   length=%d: %2x %2x %2x\n", __func__, prelength, length, tmbuff[length-2], tmbuff[length-1], tmbuff[length]);
	
	return length;
}

int RunMjpegDecoder(struct MainInfo *maininfo)
{
	int ret,i;
	int endofstream = 1, mjpeglength, error_flag = 0;
	FILE *jpg_file;
	char fixdata[25] = {0xFF, 0xD8, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x02, 0x58, 0x03, 0x20, 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF, 0xDB , 0x00, 0x43};


	mjpeglength = GetExifLength(maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer, maininfo) + 5;
	
	if(RequestVideoStreamBuffer(maininfo->pVideo,
								mjpeglength,  //maininfo->bufferInfo[maininfo->videoBuffer.index].length
								(char**)&maininfo->mjpegBuff,
								&maininfo->mjpeBufLen,
								(char**)&maininfo->mjpegRingBuff,
								&maininfo->mjpeRingBufLen,
								0))
	{
		printf("%s: Request Video Stream Buffer failed\n", __func__);
		return -1;
	}
	//printf("%s: Request Video Stream Buffer ok, mjpeglength=%d mjpebuflen=%d, mjperingBufLen=%d\n", __func__, mjpeglength, maininfo->mjpeBufLen, maininfo->mjpeRingBufLen);

#if 1
	memcpy(maininfo->mjpegBuffTmp + 25, maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer + 25, 65);
	memcpy(maininfo->mjpegBuffTmp + 94, maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer + 90, mjpeglength - 94);
	
	if(maininfo->mjpeBufLen >= mjpeglength)
		memcpy(maininfo->mjpegBuff, maininfo->mjpegBuffTmp, mjpeglength);
	else{
		//printf("%s: Request Video Stream Buffer ok, mjpeglength=%d mjpebuflen=%d, mjperingBufLen=%d\n", __func__, mjpeglength, maininfo->mjpeBufLen, maininfo->mjpeRingBufLen);
		memcpy(maininfo->mjpegBuff, maininfo->mjpegBuffTmp, maininfo->mjpeBufLen);
		memcpy(maininfo->mjpegRingBuff, maininfo->mjpegBuffTmp + maininfo->mjpeBufLen, mjpeglength-maininfo->mjpeBufLen);
	}
#else
	//FFDB-DQT定义量化表
	//FF DB 00 43 00 03 02 02 03 02 02 03 03 03 03 04 03 03 04 05 08 05 05 04 04 05 0A 07 07 06 08 0C 0A 0C 0C 0B 0A 0B 0B 0D 0E 12 10 0D 0E 11 0E 0B 0B 10 16 10 11 13 14 15 15 15 0C 0F 17 18 16 14 18 12 14 15 14 
	//FF DB 00 43 01 03 04 04 05 04 05 09 05 05 09 14 0D 0B 0D 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14
	if(maininfo->mjpeRingBufLen == 0){
		memcpy(maininfo->mjpegBuff, fixdata, sizeof(fixdata));
		maininfo->mjpegBuff[90] = 0xFF;
		maininfo->mjpegBuff[91] = 0xDB;
		maininfo->mjpegBuff[92] = 0x00;
		maininfo->mjpegBuff[93] = 0x43;
		memcpy(maininfo->mjpegBuff + 25, maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer + 25, 65);
		memcpy(maininfo->mjpegBuff + 94, maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer + 90, mjpeglength - 94);
	}else{
		printf("%s: Request Video Stream Buffer ok, mjpeglength=%d mjpebuflen=%d, mjperingBufLen=%d\n", __func__, mjpeglength, maininfo->mjpeBufLen, maininfo->mjpeRingBufLen);
		if(maininfo->mjpeBufLen > 94){
			memcpy(maininfo->mjpegBuff, fixdata, sizeof(fixdata));
			maininfo->mjpegBuff[90] = 0xFF;
			maininfo->mjpegBuff[91] = 0xDB;
			maininfo->mjpegBuff[92] = 0x00;
			maininfo->mjpegBuff[93] = 0x43;
			
			memcpy(maininfo->mjpegBuff + 25, maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer + 25, 65);
			memcpy(maininfo->mjpegBuff + 94, maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer + 90, maininfo->mjpeBufLen - 94);
			memcpy(maininfo->mjpegRingBuff, maininfo->bufferInfo[maininfo->videoBuffer.index].imageBuffer + maininfo->mjpeBufLen - 4, maininfo->mjpeRingBufLen);
		}else{	//mjpeBufLen小于等于94的情况直接放弃解码
			error_flag = 1;
		}
	}
#endif


	
	
	maininfo->DataInfo.pData = maininfo->mjpegBuff;
    maininfo->DataInfo.nLength = mjpeglength;
    if(SubmitVideoStreamData(maininfo->pVideo, &maininfo->DataInfo, 0))
    {
        printf("%s: #####Error: Submit Video Stream Data failed!\n", __func__);
        //return -1;
    }


//	jpg_file=fopen("./out.jpg", "wb");
//	fwrite(maininfo->mjpegBuff, mjpeglength, 1, jpg_file);
//	fclose(jpg_file);


	
    ret = DecodeVideoStream(maininfo->pVideo, endofstream/*eos*/, 0/*key frame only*/, 0/*drop b frame*/, 0/*current time*/);
	maininfo->videoPicture = RequestPicture(maininfo->pVideo, 0);
    if (maininfo->videoPicture == NULL){
        printf("%s: decoder fail, ret=%d\n", __func__, ret);
		error_flag = 1;
        //return -1;
    }
	
#if YUV420PFILE
	savePicture(maininfo);
#endif

	//NV21(YVU420SP) -> H264 -> Rtmp
	if(error_flag == 1)
		return 0;
	
	
	ret = RunEncode(maininfo);


	
	ReturnPicture(maininfo->pVideo, maininfo->videoPicture);
	maininfo->videoPicture = NULL;
	
	return ret;
}

void ExitMjpegDecoder(struct MainInfo *maininfo)
{
#if YUV420PFILE
	fclose(maininfo->yuv420p_file);
#endif
	DestroyVideoDecoder(maininfo->pVideo);
	CdcMemClose(maininfo->vConfig.memops);
	free(maininfo->mjpegBuffTmp);
}



/*
0、安全退出、稳定支架
1、优化h264带宽和显示效果

*/















