//串口相关的头文件  
#include<stdio.h>      /*标准输入输出定义*/  
#include<stdlib.h>     /*标准函数库定义*/  
#include<unistd.h>     /*Unix 标准函数定义*/  
#include<sys/types.h>   
#include<sys/stat.h>     
#include<fcntl.h>      /*文件控制定义*/  
#include<termios.h>    /*PPSIX 终端控制定义*/  
#include<errno.h>      /*错误号定义*/  
#include<string.h>  
#include <pthread.h>
#include <sched.h>

#include "lin_aurt.h" 
#include "aurt.h" 
#include "socket_server.h"

#define DELAY 16500		//28000
#define THANDEV "/dev/ttySC4"


struct MarkMem MarkMemValue = {
	.ReAirLamp = 0,
	.ACLamp = 0,
	.FrontLamp = 0,
	.RearLamp = 0,
	
	.ACMark = 0,
	.AutoMark = 0,
	.HumMark = 0,
	.DfrMark = 0,
	.VentMark = 0,
	.FootMark = 0,
	.FanGradeMark = 0,
	.FanMark = 0,
	
	.IGOFFSignal = 0,
	.BackLight = 0,
	.TemNum = 25.0,
};

int rcv_flag;
int NumTube_AC[10]={0xBE, 0x28, 0xF4, 0xF9, 0x6B, 0xDB, 0xDE, 0xA8, 0xFE, 0xFB};
static int Hardwaredete;

static int rcv_checksum(unsigned char *data)
{
	int csum = 0;
	int i;

	for (i=1; i < 10; i++) {
		csum += data[i];
		if (csum > 255)
			csum -= 255;
	}
	if((~csum & 0xff) == data[10])
		return TRUE;
	
	return FALSE;
} 

static unsigned char sllin_checksum(unsigned char *data)
{
	int csum = 0;
	int i;

	for (i=1; i < 10; i++) {
		csum += data[i];
		if (csum > 255)
			csum -= 255;
	}
	//printf("csum=%x\n",~csum & 0xff); 
	return (~csum & 0xff);
} 

static int send_data(int fd, char *send_buf1,int send_len)
{
	int len;
	char send_buf=0;

	//printf("4800\n"); 
    if (UART0_Set(fd,4800,0,8,1,'N') == FALSE)    //4800情况下发送0x00                                                     
		printf("UART0_Set failed!\n"); 
	//printf("send_buf,1\n"); 
 	len = UART0_Send(fd,&send_buf,1);
	if(len <= 0)  
		printf("send data failed!\n");

	usleep(2000);
	//printf("9600\n"); 
    if (UART0_Set(fd,9600,0,8,1,'N') == FALSE)     //改回9600发送真正的数据                                                
		printf("UART0_Set failed!\n"); 
	//printf("send_buf1,send_len\n"); 	
 	len = UART0_Send(fd,send_buf1,send_len);  
	if(len <= 0)  
		printf("send data failed!\n");
}

static int Parsingkey(char *rcv_buf, struct RecvMem *KeyMemValuetmp)
{
	int i;

	//if(rcv_checksum(rcv_buf) != 0)
	//{
	//	//printf("rcv checks fail\n");
	//	return FALSE;
	//}
	if(((rcv_buf[10] == 0) && (rcv_buf[9] == 0) && (rcv_buf[8] == 0)) || (rcv_buf[1] != 115)){
		return FALSE;
	}
	
	//printf("rcv_buf:");
	//for(i=1; i<11; i++)
	//	printf(" %d",rcv_buf[i]);
	//printf("\n");	
	
	Hardwaredete = 0;

	if(rcv_flag == 0)
	{
		switch(rcv_buf[4])
		{
			case 128:
				KeyMemValuetmp->ReAirKey = 1;
				rcv_flag = 1;
				SocketServerSend("ReAirKey", 1);
				//printf("KeyMemValuetmp.ReAirKey\n");
				break;
			case 16:
				KeyMemValuetmp->ACKey = 1;
				rcv_flag = 1;
				SocketServerSend("ACKey", 1);
				//printf("KeyMemValuetmp.ACKey\n");
				break;
			case 1:
				KeyMemValuetmp->OffKey = 1;
				rcv_flag = 1;
				SocketServerSend("OffKey", 1);
				//printf("KeyMemValuetmp.OffKey\n");
				break;
			case 2:
				KeyMemValuetmp->AutoKey = 1;
				rcv_flag = 1;
				SocketServerSend("AutoKey", 1);
				//printf("KeyMemValuetmp.AutoKey\n");
				break;
			case 8:
				KeyMemValuetmp->FrontKey = 1;
				rcv_flag = 1;
				SocketServerSend("FrontKey", 1);
				//printf("KeyMemValuetmp.FrontKey\n");
				break;
			case 4:
				KeyMemValuetmp->HumKey = 1;
				rcv_flag = 1;
				SocketServerSend("HumKey", 1);
				//printf("KeyMemValuetmp.HumKey\n");
				break;
			case 32:
				KeyMemValuetmp->FanKeyUp = 1;
				rcv_flag = 1;
				SocketServerSend("FanKeyUp", 1);
				//printf("KeyMemValuetmp.FanKeyUp\n");
				break;
			case 64:
				KeyMemValuetmp->FanKeyDown = 1;
				rcv_flag = 1;
				SocketServerSend("FanKeyDown", 1);
				//printf("KeyMemValuetmp.FanKeyDown\n");
				break;			
			default :
				break;
		}
		switch(rcv_buf[5])
		{
			case 128:
				KeyMemValuetmp->RearKey = 1;
				rcv_flag = 1;
				SocketServerSend("RearKey", 1);
				//printf("KeyMemValuetmp.RearKey\n");
				break;
			case 8:
				KeyMemValuetmp->TemKeyUp = 1;
				rcv_flag = 1;
				SocketServerSend("TemKeyUp", 1);
				//printf("KeyMemValuetmp.TemKeyUp\n");
				break;
			case 4:
				KeyMemValuetmp->TemKeyDown = 1;
				rcv_flag = 1;
				SocketServerSend("TemKeyDown", 1);
				//printf("KeyMemValuetmp.TemKeyDown\n");
				break;
			default :
				break;
		}
	}
	if(rcv_flag == 1)
	{
		if(rcv_buf[4] == 0)
		{
			if(KeyMemValuetmp->ReAirKey)
			{
				KeyMemValuetmp->ReAirKey = 0;
				rcv_flag = 0;
				SocketServerSend("ReAirKey", 2);
				//printf("KeyMemValuetmp.ReAirKey\n");
			}
			if(KeyMemValuetmp->ACKey)
			{
				KeyMemValuetmp->ACKey = 0;
				rcv_flag = 0;
				SocketServerSend("ACKey", 2);
				//printf("KeyMemValuetmp.ACKey\n");
			}
			if(KeyMemValuetmp->OffKey)
			{
				KeyMemValuetmp->OffKey = 0;
				rcv_flag = 0;
				SocketServerSend("OffKey", 2);
				//printf("KeyMemValuetmp.OffKey\n");
			}
			if(KeyMemValuetmp->AutoKey)
			{
				KeyMemValuetmp->AutoKey = 0;
				rcv_flag = 0;
				SocketServerSend("AutoKey", 2);
				//printf("KeyMemValuetmp.AutoKey\n");
			}
			if(KeyMemValuetmp->FrontKey)
			{
				KeyMemValuetmp->FrontKey = 0;
				rcv_flag = 0;
				SocketServerSend("FrontKey", 2);
				//printf("KeyMemValuetmp.FrontKey\n");
			}
			if(KeyMemValuetmp->HumKey)
			{
				KeyMemValuetmp->HumKey = 0;
				rcv_flag = 0;
				SocketServerSend("HumKey", 2);
				//printf("KeyMemValuetmp.HumKey\n");
			}
			if(KeyMemValuetmp->FanKeyUp)
			{
				KeyMemValuetmp->FanKeyUp = 0;
				rcv_flag = 0;
				SocketServerSend("FanKeyUp", 2);
				//printf("KeyMemValuetmp.FanKeyUp\n");
			}
			if(KeyMemValuetmp->FanKeyDown)
			{
				KeyMemValuetmp->FanKeyDown = 0;
				rcv_flag = 0;
				SocketServerSend("FanKeyDown", 2);
				//printf("KeyMemValuetmp.FanKeyDown\n");
			}
		}
		if(rcv_buf[5] == 0)
		{
			if(KeyMemValuetmp->RearKey)
			{
				KeyMemValuetmp->RearKey = 0;
				rcv_flag = 0;
				SocketServerSend("RearKey", 2);
				//printf("KeyMemValuetmp.RearKey\n");
			}
			if(KeyMemValuetmp->TemKeyUp)
			{
				KeyMemValuetmp->TemKeyUp = 0;
				rcv_flag = 0;
				SocketServerSend("TemKeyUp", 2);
				//printf("KeyMemValuetmp.TemKeyUp\n");
			}
			if(KeyMemValuetmp->TemKeyDown)
			{
				KeyMemValuetmp->TemKeyDown = 0;
				rcv_flag = 0;
				SocketServerSend("TemKeyDown", 2);
				//printf("KeyMemValuetmp.TemKeyDown\n");
			}
		}			
	}		
	//KeyMemValuetmp->SensorTemp = rcv_buf[8];
	//KeyMemValuetmp->SensorVolt = rcv_buf[9];
	

	
	return 0;
}

int Receiving_data( struct MarkMem *MarMemValuetmp, unsigned char *send_buf_F0tmp)
{
	int numtmp, Tenplace, Bit, decimal,i; 
	
	numtmp = MarMemValuetmp->TemNum * 10;
	Tenplace = numtmp/100;
	Bit = (numtmp%100)/10;
	decimal = (numtmp%100)%10;
	send_buf_F0tmp[2] = NumTube_AC[Tenplace];
	send_buf_F0tmp[3] = NumTube_AC[Bit] | 1;
	send_buf_F0tmp[4] = NumTube_AC[decimal];	


	//printf("numtmp = %d Tenplace=%d Bit=%d decimal=%d\n",numtmp,Tenplace,Bit,decimal);

	if(MarMemValuetmp->IGOFFSignal)		//开关机
		send_buf_F0tmp[8] = send_buf_F0tmp[8] | (1 << 2);
	else
		send_buf_F0tmp[8] = send_buf_F0tmp[8] & ~(1 << 2);	
	
	if(MarMemValuetmp->BackLight)	//背光,关背光的时候把数字显示也清除掉
		send_buf_F0tmp[8] = send_buf_F0tmp[8] | (1 << 1);
	else{
		send_buf_F0tmp[8] = send_buf_F0tmp[8] & ~(1 << 1);
		send_buf_F0tmp[2] = 0;
		send_buf_F0tmp[3] = 0;
		send_buf_F0tmp[4] = 0;
	}

	if(MarMemValuetmp->ReAirLamp)	//循环空气灯
		send_buf_F0tmp[5] = send_buf_F0tmp[5] | (1 << 5);
	else
		send_buf_F0tmp[5] = send_buf_F0tmp[5] & ~(1 << 5);

	if(MarMemValuetmp->ACLamp)		//AC灯
		send_buf_F0tmp[5] = send_buf_F0tmp[5] | (1 << 2);
	else
		send_buf_F0tmp[5] = send_buf_F0tmp[5] & ~(1 << 2);
    
	if(MarMemValuetmp->FrontLamp)	//前除雾器指示灯
		send_buf_F0tmp[5] = send_buf_F0tmp[5] | (1 << 7);
	else
		send_buf_F0tmp[5] = send_buf_F0tmp[5] & ~(1 << 7);
    
	if(MarMemValuetmp->RearLamp)	//后除雾器指示灯
		send_buf_F0tmp[5] = send_buf_F0tmp[5] | (1 << 6);
	else
		send_buf_F0tmp[5] = send_buf_F0tmp[5] & ~(1 << 6);
    
	if(MarMemValuetmp->ACMark)		//A/C 图标
		send_buf_F0tmp[7] = send_buf_F0tmp[7] | (1 << 4);
	else
		send_buf_F0tmp[7] = send_buf_F0tmp[7] & ~(1 << 4);
    
	if(MarMemValuetmp->AutoMark)	//Auto 图标
		send_buf_F0tmp[7] = send_buf_F0tmp[7] | (1 << 3);
	else
		send_buf_F0tmp[7] = send_buf_F0tmp[7] & ~(1 << 3);
    
	if(MarMemValuetmp->HumMark)		//小人 图标
		send_buf_F0tmp[8] = send_buf_F0tmp[8] | (1 << 7);
	else
		send_buf_F0tmp[8] = send_buf_F0tmp[8] & ~(1 << 7);
    
	if(MarMemValuetmp->DfrMark)		//除雾器图标
		send_buf_F0tmp[7] = send_buf_F0tmp[7] | (1 << 5);
	else
		send_buf_F0tmp[7] = send_buf_F0tmp[7] & ~(1 << 5);
    
	if(MarMemValuetmp->VentMark)	//Vent 图标
		send_buf_F0tmp[7] = send_buf_F0tmp[7] | (1 << 6);
	else
		send_buf_F0tmp[7] = send_buf_F0tmp[7] & ~(1 << 6);
    
	if(MarMemValuetmp->FootMark)	//Foot 图标
		send_buf_F0tmp[7] = send_buf_F0tmp[7] | (1 << 7);
	else
		send_buf_F0tmp[7] = send_buf_F0tmp[7] & ~(1 << 7);	
    
	if(MarMemValuetmp->FanMark)		//小风扇图标
		send_buf_F0tmp[7] = send_buf_F0tmp[7] | (1 << 0);
	else
		send_buf_F0tmp[7] = send_buf_F0tmp[7] & ~(1 << 0);	
	
	
	switch(MarMemValuetmp->FanGradeMark)
	{
		case 0:
			send_buf_F0tmp[6] = 0x0;
			break;
		case 1:
			send_buf_F0tmp[6] = 0x1;
			break;
		case 2:
			send_buf_F0tmp[6] = 0x3;
			break;
		case 3:
			send_buf_F0tmp[6] = 0x7;
			break;
		case 4:
			send_buf_F0tmp[6] = 0xF;
			break;
		case 5:
			send_buf_F0tmp[6] = 0x1F;
			break;	
		case 6:
			send_buf_F0tmp[6] = 0x3F;
			break;
		case 7:
			send_buf_F0tmp[6] = 0x7F;
			break;	
		case 8:
			send_buf_F0tmp[6] = 0xFF;
			break;
		default :
			break;			
	}

	send_buf_F0tmp[10] = sllin_checksum(send_buf_F0tmp);
	
	//for(i=0; i<11; i++)
	//	printf("send_buf_F0tmp[%d] = %d\n",i,send_buf_F0tmp[i]);
	
	return 0;
}

int main(int argc, char **argv)
{  
    int fd;                            //文件描述符  
    int len;   
	pthread_t tidp;
	struct RecvMem KeyMemValue;

    char rcv_buf[11];         
	char send_buf_73[2]={0x55,0x73};
	unsigned char send_buf_F0[11]={0x55,0xF0};
	unsigned char send_buf_B1[11]={0x55,0xB1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	send_buf_B1[10] = sllin_checksum(send_buf_B1);
	send_buf_F0[10] = sllin_checksum(send_buf_F0);

	memset(&KeyMemValue, 0, sizeof(struct RecvMem));
	//nice(-20); 
	pid_t pid = getpid();
	struct sched_param param;
	param.sched_priority = sched_get_priority_max(SCHED_FIFO);   // 也可用SCHED_RR
	sched_setscheduler(pid, SCHED_FIFO, &param);                 // 设置当前进程
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);   // 设置当前线程

	/* creat pthread */
	if ((pthread_create(&tidp, NULL, SocketServerPthread, NULL)) == -1)
	{
		printf("create pthread error!\n");
		return  0; 
	}
	
	fd = UART0_Open(fd,THANDEV); //打开串口，返回文件描述符 
	if(fd == FALSE)
	{
		sleep(10);
		return  0;
	}
	
	while (1)
	{
		while (1)
		{
			//printf("~~~~~~~~~~~~~~~\n");  
			if(Hardwaredete >= 15){
				UART0_Close(fd);
				while(1)
				{
					sleep(100);
				}
			}

			send_data(fd, send_buf_73,2);				//send PID 0x73
			len = UART0_Recv(fd, rcv_buf,11);  			//rcv data
			if(len <= 0)  
			{
				tcflush(fd,TCIOFLUSH);
				UART0_Close(fd);
				Hardwaredete++;
				printf("cannot receive data:%d\n",Hardwaredete);
				usleep(100000);
				
				fd = UART0_Open(fd,THANDEV); 			//打开串口，返回文件描述符 
				if(fd == FALSE){
					printf("UART0_Open fail\n");
				}
				break;
			}
			
			if(rcv_buf[7] == 136){
				printf("LIN Error State\n");
			}else{
				Parsingkey(rcv_buf, &KeyMemValue);
				Receiving_data(&MarkMemValue, send_buf_F0);
			}
			usleep(3500);
			send_data(fd, send_buf_F0,11);				//send PID 0xF0	

			usleep(DELAY);
			send_data(fd, send_buf_B1,11);				//send PID 0xB1
			usleep(DELAY);
			tcflush(fd,TCIOFLUSH);
		}
	}

	UART0_Close(fd);
	return  0; 
}
