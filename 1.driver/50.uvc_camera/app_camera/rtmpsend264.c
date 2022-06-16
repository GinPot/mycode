/**
 * Simplest Librtmp Send 264
 *
 * �����裬����
 * leixiaohua1020@126.com
 * zhanghuicuc@gmail.com
 * �й���ý��ѧ/���ֵ��Ӽ���
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * ���������ڽ��ڴ��е�H.264����������RTMP��ý���������
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <rtmp.h>   
#include <rtmp_sys.h>   
#include <amf.h>  
#include "rtmpsend264.h" 

//�����ͷ���ȣ�RTMP_MAX_HEADER_SIZE=18
#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)
//�洢Nal��Ԫ���ݵ�buffer��С
#define BUFFER_SIZE 32768
//��ѰNal��Ԫʱ��һЩ��־
#define GOT_A_NAL_CROSS_BUFFER BUFFER_SIZE+1
#define GOT_A_NAL_INCLUDE_A_BUFFER BUFFER_SIZE+2
#define NO_MORE_BUFFER_TO_READ BUFFER_SIZE+3

/**
 * _NaluUnit
 * �ڲ��ṹ�塣�ýṹ����Ҫ���ڴ洢�ʹ���Nal��Ԫ�����͡���С������
 */ 
typedef struct _NaluUnit  
{  
	int type;  
    int size;  
	unsigned char *data;  
}NaluUnit;



enum  
{  
	 VIDEO_CODECID_H264 = 7,  
};  

static UINT Ue(BYTE *pBuff, UINT nLen, UINT (*nStartBit))
{
	//����0bit�ĸ���
	UINT nZeroNum = 0;
	while ((*nStartBit) < nLen * 8)
	{
		if (pBuff[(*nStartBit) / 8] & (0x80 >> ((*nStartBit) % 8))) //&:��λ�룬%ȡ��
		{
			break;
		}
		nZeroNum++;
		((*nStartBit))++;
	}
	((*nStartBit)) ++;


	//������
	DWORD dwRet = 0;
	for (UINT i=0; i<nZeroNum; i++)
	{
		dwRet <<= 1;
		if (pBuff[(*nStartBit) / 8] & (0x80 >> ((*nStartBit) % 8)))
		{
			dwRet += 1;
		}
		((*nStartBit))++;
	}
	return (1 << nZeroNum) - 1 + dwRet;
}


static int Se(BYTE *pBuff, UINT nLen, UINT ((*nStartBit)))
{
	int UeVal=Ue(pBuff,nLen,nStartBit);
	double k=UeVal;
	int nValue=ceil(k/2);//ceil������ceil��������������С�ڸ���ʵ������С������ceil(2)=ceil(1.2)=cei(1.5)=2.00
	if (UeVal % 2==0)
		nValue=-nValue;
	return nValue;
}


static DWORD u(UINT BitCount,BYTE * buf,UINT ((*nStartBit)))
{
	DWORD dwRet = 0;
	for (UINT i=0; i<BitCount; i++)
	{
		dwRet <<= 1;
		if (buf[(*nStartBit) / 8] & (0x80 >> ((*nStartBit) % 8)))
		{
			dwRet += 1;
		}
		((*nStartBit))++;
	}
	return dwRet;
}

/**
 * H264��NAL��ʼ����������� 
 *
 * @param buf SPS��������
 *
 * @�޷���ֵ
 */ 
static void de_emulation_prevention(BYTE* buf,unsigned int* buf_size)
{
	int i=0,j=0;
	BYTE* tmp_ptr=NULL;
	unsigned int tmp_buf_size=0;
	int val=0;

	tmp_ptr=buf;
	tmp_buf_size=*buf_size;
	for(i=0;i<(tmp_buf_size-2);i++)
	{
		//check for 0x000003
		val=(tmp_ptr[i]^0x00) +(tmp_ptr[i+1]^0x00)+(tmp_ptr[i+2]^0x03);
		if(val==0)
		{
			//kick out 0x03
			for(j=i+2;j<tmp_buf_size-1;j++)
				tmp_ptr[j]=tmp_ptr[j+1];

			//and so we should devrease bufsize
			(*buf_size)--;
		}
	}

	return;
}

/**
 * ����SPS,��ȡ��Ƶͼ�������Ϣ 
 *
 * @param buf SPS��������
 * @param nLen SPS���ݵĳ���
 * @param width ͼ����
 * @param height ͼ��߶�

 * @�ɹ��򷵻�1 , ʧ���򷵻�0
 */ 
static int h264_decode_sps(BYTE * buf,unsigned int nLen,int *width,int *height,int *fps)
{
	UINT uStartBit=0; 
	UINT *StartBit = &uStartBit;
	int forbidden_zero_bit = 0;
	int nal_ref_idc = 0;
	int nal_unit_type = 0;
	int profile_idc = 0;
	int constraint_set0_flag = 0;
	int constraint_set1_flag = 0;
	int constraint_set2_flag = 0;
	int constraint_set3_flag = 0;
	int reserved_zero_4bits=0;
	int level_idc = 0;
	int seq_parameter_set_id = 0;
	int chroma_format_idc = 0;
	int residual_colour_transform_flag = 0;
	int bit_depth_luma_minus8=0;
	int bit_depth_chroma_minus8=0;
	int qpprime_y_zero_transform_bypass_flag=0;
	int seq_scaling_matrix_present_flag=0;
	int seq_scaling_list_present_flag[8];
	int log2_max_frame_num_minus4=0;
	int pic_order_cnt_type=0;
	int log2_max_pic_order_cnt_lsb_minus4=0;
	int delta_pic_order_always_zero_flag=0;
	int offset_for_non_ref_pic=0;
	int offset_for_top_to_bottom_field=0;
	int num_ref_frames_in_pic_order_cnt_cycle=0;
	int *offset_for_ref_frame=NULL;
	int i=0;
	int num_ref_frames=0;
	int gaps_in_frame_num_value_allowed_flag=0;
	int pic_width_in_mbs_minus1=0;
	int pic_height_in_map_units_minus1=0;
	int frame_mbs_only_flag =0;	
	int mb_adaptive_frame_field_flag=0;	
	int direct_8x8_inference_flag=0;
	int frame_cropping_flag=0;
	int frame_crop_left_offset=0;
	int frame_crop_right_offset=0;
	int frame_crop_top_offset=0;
	int frame_crop_bottom_offset=0;
	int vui_parameter_present_flag=0;	
	int aspect_ratio_info_present_flag=0;	
	int aspect_ratio_idc=0;   
	int sar_width=0;                                  
	int sar_height=0;                                      
	int overscan_info_present_flag=0; 
	int overscan_appropriate_flagu=0;                   
	int video_signal_type_present_flag=0; 
	int video_format=0;                         
	int video_full_range_flag=0;                       
	int colour_description_present_flag=0;
	int colour_primaries=0;              
	int transfer_characteristics=0;                     
	int matrix_coefficients=0;                  		
	int chroma_loc_info_present_flag=0;  
	int chroma_sample_loc_type_top_field=0;             
	int chroma_sample_loc_type_bottom_field=0;       
	int timing_info_present_flag=0;        
	int num_units_in_tick=0;                              
	int time_scale=0; 
	de_emulation_prevention(buf,&nLen);
	printf("nLen = %d\r\n",nLen);
	forbidden_zero_bit=u(1,buf,StartBit);
	nal_ref_idc=u(2,buf,StartBit);
	nal_unit_type=u(5,buf,StartBit);
	if(nal_unit_type==7)
	{
		profile_idc=u(8,buf,StartBit);
		constraint_set0_flag=u(1,buf,StartBit);//(buf[1] & 0x80)>>7;
		constraint_set1_flag=u(1,buf,StartBit);//(buf[1] & 0x40)>>6;
		constraint_set2_flag=u(1,buf,StartBit);//(buf[1] & 0x20)>>5;
		constraint_set3_flag=u(1,buf,StartBit);//(buf[1] & 0x10)>>4;
		reserved_zero_4bits=u(4,buf,StartBit);
		level_idc=u(8,buf,StartBit);

		seq_parameter_set_id=Ue(buf,nLen,StartBit);

		if( profile_idc == 100 || profile_idc == 110 ||
			profile_idc == 122 || profile_idc == 144 )
		{
			chroma_format_idc=Ue(buf,nLen,StartBit);
			if( chroma_format_idc == 3 )
				residual_colour_transform_flag=u(1,buf,StartBit);
			bit_depth_luma_minus8=Ue(buf,nLen,StartBit);
			bit_depth_chroma_minus8=Ue(buf,nLen,StartBit);
			qpprime_y_zero_transform_bypass_flag=u(1,buf,StartBit);
			seq_scaling_matrix_present_flag=u(1,buf,StartBit);

			if( seq_scaling_matrix_present_flag )
			{
				for( i = 0; i < 8; i++ ) {
					seq_scaling_list_present_flag[i]=u(1,buf,StartBit);
				}
			}
		}
		log2_max_frame_num_minus4=Ue(buf,nLen,StartBit);
		pic_order_cnt_type=Ue(buf,nLen,StartBit);
		if( pic_order_cnt_type == 0 )
			log2_max_pic_order_cnt_lsb_minus4=Ue(buf,nLen,StartBit);
		else if( pic_order_cnt_type == 1 )
		{
			delta_pic_order_always_zero_flag=u(1,buf,StartBit);
			offset_for_non_ref_pic=Se(buf,nLen,StartBit);
			offset_for_top_to_bottom_field=Se(buf,nLen,StartBit);
			num_ref_frames_in_pic_order_cnt_cycle=Ue(buf,nLen,StartBit);

			offset_for_ref_frame= (int*)malloc(num_ref_frames_in_pic_order_cnt_cycle);
			for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
				offset_for_ref_frame[i]=Se(buf,nLen,StartBit);
			free(offset_for_ref_frame);
		}
		num_ref_frames=Ue(buf,nLen,StartBit);
		gaps_in_frame_num_value_allowed_flag=u(1,buf,StartBit);
		pic_width_in_mbs_minus1=Ue(buf,nLen,StartBit);
		pic_height_in_map_units_minus1=Ue(buf,nLen,StartBit);
		
		*width=(pic_width_in_mbs_minus1+1)*16;
		*height=(pic_height_in_map_units_minus1+1)*16;

		frame_mbs_only_flag=u(1,buf,StartBit);
		if(!frame_mbs_only_flag)
			mb_adaptive_frame_field_flag=u(1,buf,StartBit);

		direct_8x8_inference_flag=u(1,buf,StartBit);
		frame_cropping_flag=u(1,buf,StartBit);
		if(frame_cropping_flag)
		{
			frame_crop_left_offset=Ue(buf,nLen,StartBit);
			frame_crop_right_offset=Ue(buf,nLen,StartBit);
			frame_crop_top_offset=Ue(buf,nLen,StartBit);
			frame_crop_bottom_offset=Ue(buf,nLen,StartBit);
		}
		vui_parameter_present_flag=u(1,buf,StartBit);
		if(vui_parameter_present_flag)
		{
			aspect_ratio_info_present_flag=u(1,buf,StartBit);              
			if(aspect_ratio_info_present_flag)
			{
				aspect_ratio_idc=u(8,buf,StartBit);   
				if(aspect_ratio_idc==255)
				{
					sar_width=u(16,buf,StartBit);                                  
					sar_height=u(16,buf,StartBit);                                      
				}
			}
			overscan_info_present_flag=u(1,buf,StartBit); 
			if(overscan_info_present_flag)
				overscan_appropriate_flagu=u(1,buf,StartBit);                   
			video_signal_type_present_flag=u(1,buf,StartBit); 
			if(video_signal_type_present_flag)
			{
				video_format=u(3,buf,StartBit);                         
				video_full_range_flag=u(1,buf,StartBit);                       
				colour_description_present_flag=u(1,buf,StartBit);
				if(colour_description_present_flag)
				{
					colour_primaries=u(8,buf,StartBit);              
					transfer_characteristics=u(8,buf,StartBit);                     
					matrix_coefficients=u(8,buf,StartBit);                  		
				}
			}
			chroma_loc_info_present_flag=u(1,buf,StartBit);  
			if(chroma_loc_info_present_flag)
			{
				chroma_sample_loc_type_top_field=Ue(buf,nLen,StartBit);             
				chroma_sample_loc_type_bottom_field=Ue(buf,nLen,StartBit);       
			}
			timing_info_present_flag=u(1,buf,StartBit);        
			if(timing_info_present_flag)
			{
				num_units_in_tick=u(32,buf,StartBit);                              
				time_scale=u(32,buf,StartBit);    
				*fps=time_scale/(2*num_units_in_tick);
			}
		}
		return true;
	}
	else
		return false;
}

//�����ֽ���ת��
static char * put_byte( char *output, uint8_t nVal )    
{    
	output[0] = nVal;    
	return output+1;    
}   

static char * put_be16(char *output, uint16_t nVal )    
{    
	output[1] = nVal & 0xff;    
	output[0] = nVal >> 8;    
	return output+2;    
}  

static char * put_be24(char *output,uint32_t nVal )    
{    
	output[2] = nVal & 0xff;    
	output[1] = nVal >> 8;    
	output[0] = nVal >> 16;    
	return output+3;    
}    
static char * put_be32(char *output, uint32_t nVal )    
{    
	output[3] = nVal & 0xff;    
	output[2] = nVal >> 8;    
	output[1] = nVal >> 16;    
	output[0] = nVal >> 24;    
	return output+4;    
}    
static char *  put_be64( char *output, uint64_t nVal )    
{    
	output=put_be32( output, nVal >> 32 );    
	output=put_be32( output, nVal );    
	return output;    
}  

static char * put_amf_string( char *c, const char *str )    
{    
	uint16_t len = strlen( str );    
	c=put_be16( c, len );    
	memcpy(c,str,len);    
	return c+len;    
}    
static char * put_amf_double( char *c, double d )    
{    
	*c++ = AMF_NUMBER;  /* type: Number */    
	{    
		unsigned char *ci, *co;    
		ci = (unsigned char *)&d;    
		co = (unsigned char *)c;    
		co[0] = ci[7];    
		co[1] = ci[6];    
		co[2] = ci[5];    
		co[3] = ci[4];    
		co[4] = ci[3];    
		co[5] = ci[2];    
		co[6] = ci[1];    
		co[7] = ci[0];    
	}    
	return c+8;    
}  


unsigned int  m_nFileBufSize; 
unsigned int  nalhead_pos;
RTMP* m_pRtmp;  

unsigned char *m_pFileBuf;  
unsigned char *m_pFileBuf_tmp;
unsigned char* m_pFileBuf_tmp_old;	//used for realloc

/**
 * ��ʼ�������ӵ�������
 *
 * @param url �������϶�Ӧwebapp�ĵ�ַ
 *					
 * @�ɹ��򷵻�1 , ʧ���򷵻�0
 */ 
int RTMP264_Connect(const char* url)  
{  
	nalhead_pos=0;
	m_nFileBufSize=BUFFER_SIZE;
	m_pFileBuf=(unsigned char*)malloc(BUFFER_SIZE);
	m_pFileBuf_tmp=(unsigned char*)malloc(BUFFER_SIZE);

	m_pRtmp = RTMP_Alloc();
	RTMP_Init(m_pRtmp);
	/*����URL*/
	if (RTMP_SetupURL(m_pRtmp,(char*)url) == FALSE)
	{
		RTMP_Free(m_pRtmp);
		return false;
	}
	/*���ÿ�д,��������,�����������������ǰʹ��,������Ч*/
	RTMP_EnableWrite(m_pRtmp);
	/*���ӷ�����*/
	if (RTMP_Connect(m_pRtmp, NULL) == FALSE) 
	{
		RTMP_Free(m_pRtmp);
		return false;
	} 

	/*������*/
	if (RTMP_ConnectStream(m_pRtmp,0) == FALSE)
	{
		RTMP_Close(m_pRtmp);
		RTMP_Free(m_pRtmp);
		return false;
	}
	return true;  
}  


/**
 * �Ͽ����ӣ��ͷ���ص���Դ��
 *
 */    
void RTMP264_Close()  
{  
	if(m_pRtmp)  
	{  
		RTMP_Close(m_pRtmp);  
		RTMP_Free(m_pRtmp);  
		m_pRtmp = NULL;  
	}  
   
	if (m_pFileBuf != NULL)
	{  
		free(m_pFileBuf);
	}  
	if (m_pFileBuf_tmp != NULL)
	{  
		free(m_pFileBuf_tmp);
	}
} 

/**
 * ����RTMP���ݰ�
 *
 * @param nPacketType ��������
 * @param data �洢��������
 * @param size ���ݴ�С
 * @param nTimestamp ��ǰ����ʱ���
 *
 * @�ɹ��򷵻� 1 , ʧ���򷵻�һ��С��0����
 */
static int SendPacket(unsigned int nPacketType,unsigned char *data,unsigned int size,unsigned int nTimestamp)  
{  
	RTMPPacket* packet;
	/*������ڴ�ͳ�ʼ��,lenΪ���峤��*/
	packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+size);
	memset(packet,0,RTMP_HEAD_SIZE);
	/*�����ڴ�*/
	packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
	packet->m_nBodySize = size;
	memcpy(packet->m_body,data,size);
	packet->m_hasAbsTimestamp = 0;
	packet->m_packetType = nPacketType; /*�˴�Ϊ����������һ������Ƶ,һ������Ƶ*/
	packet->m_nInfoField2 = m_pRtmp->m_stream_id;
	packet->m_nChannel = 0x04;

	packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
	if (RTMP_PACKET_TYPE_AUDIO ==nPacketType && size !=4)
	{
		packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	}
	packet->m_nTimeStamp = nTimestamp;
	/*����*/
	int nRet =0;
	if (RTMP_IsConnected(m_pRtmp))
	{
		nRet = RTMP_SendPacket(m_pRtmp,packet,TRUE); /*TRUEΪ�Ž����Ͷ���,FALSE�ǲ��Ž����Ͷ���,ֱ�ӷ���*/
	}
	/*�ͷ��ڴ�*/
	free(packet);
	return nRet;  
}  

/**
 * ������Ƶ��sps��pps��Ϣ
 *
 * @param pps �洢��Ƶ��pps��Ϣ
 * @param pps_len ��Ƶ��pps��Ϣ����
 * @param sps �洢��Ƶ��pps��Ϣ
 * @param sps_len ��Ƶ��sps��Ϣ����
 *
 * @�ɹ��򷵻� 1 , ʧ���򷵻�0
 */
static int SendVideoSpsPps(unsigned char *pps,int pps_len,unsigned char * sps,int sps_len)
{
	RTMPPacket * packet=NULL;//rtmp���ṹ
	unsigned char * body=NULL;
	int i;
	packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+1024);
	//RTMPPacket_Reset(packet);//����packet״̬
	memset(packet,0,RTMP_HEAD_SIZE+1024);
	packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
	body = (unsigned char *)packet->m_body;
	i = 0;
	body[i++] = 0x17;
	body[i++] = 0x00;

	body[i++] = 0x00;
	body[i++] = 0x00;
	body[i++] = 0x00;

	/*AVCDecoderConfigurationRecord*/
	body[i++] = 0x01;
	body[i++] = sps[1];
	body[i++] = sps[2];
	body[i++] = sps[3];
	body[i++] = 0xff;

	/*sps*/
	body[i++]   = 0xe1;
	body[i++] = (sps_len >> 8) & 0xff;
	body[i++] = sps_len & 0xff;
	memcpy(&body[i],sps,sps_len);
	i +=  sps_len;

	/*pps*/
	body[i++]   = 0x01;
	body[i++] = (pps_len >> 8) & 0xff;
	body[i++] = (pps_len) & 0xff;
	memcpy(&body[i],pps,pps_len);
	i +=  pps_len;

	packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
	packet->m_nBodySize = i;
	packet->m_nChannel = 0x04;
	packet->m_nTimeStamp = 0;
	packet->m_hasAbsTimestamp = 0;
	packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet->m_nInfoField2 = m_pRtmp->m_stream_id;

	/*���÷��ͽӿ�*/
	int nRet = RTMP_SendPacket(m_pRtmp,packet,TRUE);
	free(packet);    //�ͷ��ڴ�
	return nRet;
}

/**
 * ����H264����֡
 *
 * @param data �洢����֡����
 * @param size ����֡�Ĵ�С
 * @param bIsKeyFrame ��¼��֡�Ƿ�Ϊ�ؼ�֡
 * @param nTimeStamp ��ǰ֡��ʱ���
 *
 * @�ɹ��򷵻� 1 , ʧ���򷵻�0
 */
static int SendH264Packet(unsigned char *data,unsigned int size,int bIsKeyFrame,unsigned int nTimeStamp)  
{  
	int i = 0;;

	if(data == NULL && size<11){  
		return false;  
	}  

	unsigned char *body = (unsigned char*)malloc(size+9);  
	memset(body,0,size+9);

 
	if(bIsKeyFrame){  
		body[i++] = 0x17;// 1:Iframe  7:AVC   
		body[i++] = 0x01;// AVC NALU   
		body[i++] = 0x00;  
		body[i++] = 0x00;  
		body[i++] = 0x00;  


		// NALU size   
		body[i++] = size>>24 &0xff;  
		body[i++] = size>>16 &0xff;  
		body[i++] = size>>8 &0xff;  
		body[i++] = size&0xff;
		// NALU data   
		memcpy(&body[i],data,size);  
		SendVideoSpsPps(metaData.Pps,metaData.nPpsLen,metaData.Sps,metaData.nSpsLen);
	}else{  
		body[i++] = 0x27;// 2:Pframe  7:AVC   
		body[i++] = 0x01;// AVC NALU   
		body[i++] = 0x00;  
		body[i++] = 0x00;  
		body[i++] = 0x00;  


		// NALU size   
		body[i++] = size>>24 &0xff;  
		body[i++] = size>>16 &0xff;  
		body[i++] = size>>8 &0xff;  
		body[i++] = size&0xff;
		// NALU data   
		memcpy(&body[i],data,size);  
	}  
	

	int bRet = SendPacket(RTMP_PACKET_TYPE_VIDEO,body,i+size,nTimeStamp);  

	free(body);  

	return bRet;  
} 

/**
 * ���ڴ��ж�ȡ����һ��Nal��Ԫ
 *
 * @param nalu �洢nalu����
 * @param read_buffer �ص������������ݲ����ʱ��ϵͳ���Զ����øú�����ȡ�������ݡ�
 *					2���������ܣ�
 *					uint8_t *buf���ⲿ���������õ�ַ
 *					int buf_size���ⲿ���ݴ�С
 *					����ֵ���ɹ���ȡ���ڴ��С
 * @�ɹ��򷵻� 1 , ʧ���򷵻�0
 */
static int ReadFirstNaluFromBuf(NaluUnit *nalu,int (*read_buffer)(uint8_t *buf, int buf_size)) 
{
	int naltail_pos=nalhead_pos;
	memset(m_pFileBuf_tmp,0,BUFFER_SIZE);
	while(nalhead_pos<m_nFileBufSize)  
	{  
		//search for nal header
		if(m_pFileBuf[nalhead_pos++] == 0x00 && 
			m_pFileBuf[nalhead_pos++] == 0x00) 
		{
			if(m_pFileBuf[nalhead_pos++] == 0x01)
				goto gotnal_head;
			else 
			{
				//cuz we have done an i++ before,so we need to roll back now
				nalhead_pos--;		
				if(m_pFileBuf[nalhead_pos++] == 0x00 && 
					m_pFileBuf[nalhead_pos++] == 0x01)
					goto gotnal_head;
				else
					continue;
			}
		}
		else 
			continue;

		//search for nal tail which is also the head of next nal
gotnal_head:
		//normal case:the whole nal is in this m_pFileBuf
		naltail_pos = nalhead_pos;  
		while (naltail_pos<m_nFileBufSize)  
		{  
			if(m_pFileBuf[naltail_pos++] == 0x00 && 
				m_pFileBuf[naltail_pos++] == 0x00 )
			{  
				if(m_pFileBuf[naltail_pos++] == 0x01)
				{
					nalu->size = (naltail_pos-3)-nalhead_pos;
					break;
				}
				else
				{
					naltail_pos--;
					if(m_pFileBuf[naltail_pos++] == 0x00 &&
						m_pFileBuf[naltail_pos++] == 0x01)
					{	
						nalu->size = (naltail_pos-4)-nalhead_pos;
						break;
					}
				}
			}  
		}

		nalu->type = m_pFileBuf[nalhead_pos]&0x1f; 
		memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu->size);
		nalu->data=m_pFileBuf_tmp;
		nalhead_pos=naltail_pos;
		return TRUE;   		
	}
}

/**
 * ���ڴ��ж�ȡ��һ��Nal��Ԫ
 *
 * @param nalu �洢nalu����
 * @param read_buffer �ص������������ݲ����ʱ��ϵͳ���Զ����øú�����ȡ�������ݡ�
 *					2���������ܣ�
 *					uint8_t *buf���ⲿ���������õ�ַ
 *					int buf_size���ⲿ���ݴ�С
 *					����ֵ���ɹ���ȡ���ڴ��С
 * @�ɹ��򷵻� 1 , ʧ���򷵻�0
 */
static int ReadOneNaluFromBuf(NaluUnit *nalu,int (*read_buffer)(uint8_t *buf, int buf_size))  
{    
	
	int naltail_pos=nalhead_pos;
	int ret;
	int nalustart;//nal�Ŀ�ʼ��ʶ���Ǽ���00
	memset(m_pFileBuf_tmp,0,BUFFER_SIZE);
	nalu->size=0;
	while(1)
	{
		if(nalhead_pos==NO_MORE_BUFFER_TO_READ)
			return FALSE;
		while(naltail_pos<m_nFileBufSize)  
		{  
			//search for nal tail
			if(m_pFileBuf[naltail_pos++] == 0x00 && 
				m_pFileBuf[naltail_pos++] == 0x00) 
			{
				if(m_pFileBuf[naltail_pos++] == 0x01)
				{	
					nalustart=3;
					goto gotnal ;
				}
				else 
				{
					//cuz we have done an i++ before,so we need to roll back now
					naltail_pos--;		
					if(m_pFileBuf[naltail_pos++] == 0x00 && 
						m_pFileBuf[naltail_pos++] == 0x01)
					{
						nalustart=4;
						goto gotnal;
					}
					else
						continue;
				}
			}
			else 
				continue;

			gotnal:	
 				/**
				 *special case1:parts of the nal lies in a m_pFileBuf and we have to read from buffer 
				 *again to get the rest part of this nal
				 */
				if(nalhead_pos==GOT_A_NAL_CROSS_BUFFER || nalhead_pos==GOT_A_NAL_INCLUDE_A_BUFFER)
				{
					nalu->size = nalu->size+naltail_pos-nalustart;
					if(nalu->size>BUFFER_SIZE)
					{
						m_pFileBuf_tmp_old=m_pFileBuf_tmp;	//// save pointer in case realloc fails
						if((m_pFileBuf_tmp = (unsigned char*)realloc(m_pFileBuf_tmp,nalu->size)) ==  NULL )
						{
							free( m_pFileBuf_tmp_old );  // free original block
							return FALSE;
						}
					}
					memcpy(m_pFileBuf_tmp+nalu->size+nalustart-naltail_pos,m_pFileBuf,naltail_pos-nalustart);
					nalu->data=m_pFileBuf_tmp;
					nalhead_pos=naltail_pos;
					return TRUE;
				}
				//normal case:the whole nal is in this m_pFileBuf
				else 
				{  
					nalu->type = m_pFileBuf[nalhead_pos]&0x1f; 
					nalu->size=naltail_pos-nalhead_pos-nalustart;
					if(nalu->type==0x06)
					{
						nalhead_pos=naltail_pos;
						continue;
					}
					memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu->size);
					nalu->data=m_pFileBuf_tmp;
					nalhead_pos=naltail_pos;
					return TRUE;    
				} 					
		}

		if(naltail_pos>=m_nFileBufSize && nalhead_pos!=GOT_A_NAL_CROSS_BUFFER && nalhead_pos != GOT_A_NAL_INCLUDE_A_BUFFER)
		{
			nalu->size = BUFFER_SIZE-nalhead_pos;
			nalu->type = m_pFileBuf[nalhead_pos]&0x1f; 
			memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu->size);
			if((ret=read_buffer(m_pFileBuf,m_nFileBufSize))<BUFFER_SIZE)
			{
				memcpy(m_pFileBuf_tmp+nalu->size,m_pFileBuf,ret);
				nalu->size=nalu->size+ret;
				nalu->data=m_pFileBuf_tmp;
				nalhead_pos=NO_MORE_BUFFER_TO_READ;
				return FALSE;
			}
			naltail_pos=0;
			nalhead_pos=GOT_A_NAL_CROSS_BUFFER;
			continue;
		}
		if(nalhead_pos==GOT_A_NAL_CROSS_BUFFER || nalhead_pos == GOT_A_NAL_INCLUDE_A_BUFFER)
		{
			nalu->size = BUFFER_SIZE+nalu->size;
				
				m_pFileBuf_tmp_old=m_pFileBuf_tmp;	//// save pointer in case realloc fails
				if((m_pFileBuf_tmp = (unsigned char*)realloc(m_pFileBuf_tmp,nalu->size)) ==  NULL )
				{
					free( m_pFileBuf_tmp_old );  // free original block
					return FALSE;
				}

			memcpy(m_pFileBuf_tmp+nalu->size-BUFFER_SIZE,m_pFileBuf,BUFFER_SIZE);
			
			if((ret=read_buffer(m_pFileBuf,m_nFileBufSize))<BUFFER_SIZE)
			{
				memcpy(m_pFileBuf_tmp+nalu->size,m_pFileBuf,ret);
				nalu->size=nalu->size+ret;
				nalu->data=m_pFileBuf_tmp;
				nalhead_pos=NO_MORE_BUFFER_TO_READ;
				return FALSE;
			}
			naltail_pos=0;
			nalhead_pos=GOT_A_NAL_INCLUDE_A_BUFFER;
			continue;
		}
	}
	return FALSE;  
} 

/**
 * ���ڴ��е�һ��H.264�������Ƶ��������RTMPЭ�鷢�͵�������
 *
 * @param read_buffer �ص������������ݲ����ʱ��ϵͳ���Զ����øú�����ȡ�������ݡ�
 *					2���������ܣ�
 *					uint8_t *buf���ⲿ���������õ�ַ
 *					int buf_size���ⲿ���ݴ�С
 *					����ֵ���ɹ���ȡ���ڴ��С
 * @�ɹ��򷵻�1 , ʧ���򷵻�0
 */ 
int RTMP264_Send(int (*read_buffer)(unsigned char *buf, int buf_size))  
{    
	int ret, i;
	uint32_t now,last_update;
	  
	memset(&metaData,0,sizeof(RTMPMetadata));
	memset(m_pFileBuf,0,BUFFER_SIZE);
	if((ret=read_buffer(m_pFileBuf,m_nFileBufSize))<0)
	{
		return FALSE;
	}

	NaluUnit naluUnit;  
	// ��ȡSPS֡   
	ReadFirstNaluFromBuf(&naluUnit,read_buffer);  
	metaData.nSpsLen = naluUnit.size;  
	metaData.Sps=NULL;
	metaData.Sps=(unsigned char*)malloc(naluUnit.size);
	memcpy(metaData.Sps,naluUnit.data,naluUnit.size);

	printf("nSpsLen size:%8d\n",naluUnit.size);
	for(i = 0; i < naluUnit.size; i++)
		printf("0x%x ", *(metaData.Sps + i));
	printf("\n");

	// ��ȡPPS֡   
	ReadOneNaluFromBuf(&naluUnit,read_buffer);  
	metaData.nPpsLen = naluUnit.size; 
	metaData.Pps=NULL;
	metaData.Pps=(unsigned char*)malloc(naluUnit.size);
	memcpy(metaData.Pps,naluUnit.data,naluUnit.size);
	
	printf("nPpsLen size:%8d\n",naluUnit.size);
	for(i = 0; i < naluUnit.size; i++)
		printf("0x%x ", *(metaData.Pps + i));
	printf("\n");

	// ����SPS,��ȡ��Ƶͼ�������Ϣ   
	int width = 0,height = 0, fps=0;  
	h264_decode_sps(metaData.Sps,metaData.nSpsLen,&width,&height,&fps);  
	//metaData.nWidth = width;  
	//metaData.nHeight = height;  
	if(fps)
		metaData.nFrameRate = fps; 
	else
		metaData.nFrameRate = 25;
	printf("fps=%d width=%d height=%d\n", metaData.nFrameRate, width, height);

	//����PPS,SPS
	//ret=SendVideoSpsPps(metaData.Pps,metaData.nPpsLen,metaData.Sps,metaData.nSpsLen);
	//if(ret!=1)
	//	return FALSE;

	unsigned int tick = 0;  
	unsigned int tick_gap = 1000/metaData.nFrameRate; 
	ReadOneNaluFromBuf(&naluUnit,read_buffer);
	int bKeyframe  = (naluUnit.type == 0x05) ? TRUE : FALSE;
	while(SendH264Packet(naluUnit.data,naluUnit.size,bKeyframe,tick))  
	{    
got_sps_pps:
		//if(naluUnit.size==8581)
			printf("NALU size:%8d\n",naluUnit.size);
		last_update=RTMP_GetTime();
		if(!ReadOneNaluFromBuf(&naluUnit,read_buffer))
				goto end;
		if(naluUnit.type == 0x07 || naluUnit.type == 0x08)
			goto got_sps_pps;
		bKeyframe  = (naluUnit.type == 0x05) ? TRUE : FALSE;
		tick +=tick_gap;
		now=RTMP_GetTime();
		msleep(tick_gap-now+last_update);  
		//msleep(40);
	}  
	end:
	free(metaData.Sps);
	free(metaData.Pps);
	return TRUE;  
}  

void send_rtmp_h264(unsigned char* buff, unsigned int size)
{
	static unsigned int tick = 0;
	int bKeyframe, type = (*(buff + 4)) & 0x1f;
	
	if(type == 0x07 || type == 0x08)
		return;
	
	if(type == 0x05)
		bKeyframe = TRUE;
	else
		bKeyframe = FALSE;
	
	SendH264Packet(buff+4, size-4, bKeyframe, tick);
	
	tick += 1000/RTMPPERFRAME;

}

/*
0x0 0x0 0x0 0x1 0x67 0x64 0x0 0x33 0xac 0x2c 0xa8 0xa 0x3 0xd2 0x6e 0xa 0xc 0xa 0x4 0x0 0x0 0x0 0x1 0x68 0xee 0x3c 0x30 
nSpsLen size: 15
0x67 0x64 0x0 0x33 0xac 0x2c 0xa8 0xa 0x3 0xd2 0x6e 0xa 0xc 0xa 0x4 
nPpsLen size: 4
0x68 0xee 0x3c 0x30
*/

int init_sps_pps_data(unsigned char* buff, int size)
{
	int i, nspslen, nppslen;
	
    //printf("sps_pps_data.nLength: %d\n", sps_pps_data.nLength);
	for(i = 0; i < size; i++)
		printf("0x%x ", *(buff + i));
	printf("\n");
	
	for(i = 3; i < size; i++){
		if(( *(buff + i + 0) == 0x00) && ( *(buff + i + 1) == 0x00) && ( *(buff + i + 2) == 0x00) && ( *(buff + i + 3) == 0x01)){
			nspslen = i - 4;
			nppslen = size - i - 4;
			break;
		}
	}
	
	if(nspslen > size)
		return -1;
	
	printf("nSpsLen size: %d   nPpsLen size: %d\n", nspslen, nppslen);
	
	// ��ȡSPS֡ 
	memset(&metaData,0,sizeof(RTMPMetadata));
	metaData.nSpsLen = nspslen;  
	metaData.Sps=NULL;
	metaData.Sps=(unsigned char*)malloc(metaData.nSpsLen);
	memcpy(metaData.Sps, buff + 4, metaData.nSpsLen);

	for(i = 0; i < metaData.nSpsLen; i++)
		printf("0x%x ", *(metaData.Sps + i));
	printf("\n");
	
	// ��ȡPPS֡  
	metaData.nPpsLen = nppslen; 
	metaData.Pps=NULL;
	metaData.Pps=(unsigned char*)malloc(metaData.nPpsLen);
	memcpy(metaData.Pps, buff + nspslen + 8, metaData.nPpsLen);

	for(i = 0; i < metaData.nPpsLen; i++)
		printf("0x%x ", *(metaData.Pps + i));
	printf("\n");
}


