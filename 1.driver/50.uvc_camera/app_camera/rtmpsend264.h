#ifndef _SPS_DECODE_H_
#define _SPS_DECODE_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "camera.h"

typedef  unsigned int UINT;
typedef  unsigned char BYTE;
typedef  unsigned long DWORD;

#define false 				-1
#define true 				1
#define RTMPPERFRAME		PERFRAME
/**
 * _RTMPMetadata
 * �ڲ��ṹ�塣�ýṹ����Ҫ���ڴ洢�ʹ���Ԫ������Ϣ
 */ 
typedef struct _RTMPMetadata  
{  
	// video, must be h264 type   
	unsigned int    nWidth;  
	unsigned int    nHeight;  
	unsigned int    nFrameRate;      
	unsigned int    nSpsLen;  
	unsigned char   *Sps;  
	unsigned int    nPpsLen;  
	unsigned char   *Pps;   
} RTMPMetadata,*LPRTMPMetadata;  
RTMPMetadata metaData;

int init_sps_pps_data(unsigned char* buff, int size);
void send_rtmp_h264(unsigned char* buff, unsigned int size);



/**
 * ��ʼ�������ӵ�������
 *
 * @param url �������϶�Ӧwebapp�ĵ�ַ
 *					
 * @�ɹ��򷵻�1 , ʧ���򷵻�0
 */ 
int RTMP264_Connect(const char* url);    
    
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
int RTMP264_Send(int (*read_buffer)(unsigned char *buf, int buf_size));

/**
 * �Ͽ����ӣ��ͷ���ص���Դ��
 *
 */    
void RTMP264_Close(); 



#endif /* _SPS_DECODE_H_ */