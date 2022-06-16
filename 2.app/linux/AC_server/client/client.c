#include<stdio.h>    
#include<stdlib.h>    
#include<netinet/in.h>    
#include<sys/socket.h>    
#include<arpa/inet.h>    
#include<string.h>    
#include<unistd.h>   
#include <pthread.h>
#include "client.h"
 
#define BUFFER_SIZE 1024    

pthread_t tidpclient;
int (*KeyProcessPoin)(struct RecvKey *);
int server_sock_fd;

struct MarkMem recmarkdata;
int MarkMemFlag=1;



void *SocketClientPthread(void *arg)  
{    
	struct RecvKey recvkeyvalue;
	
    struct sockaddr_in server_addr;    
    server_addr.sin_family = AF_INET;    
    server_addr.sin_port = htons(11332);    
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");    
    bzero(&(server_addr.sin_zero), 8);    

    server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);    
    if(server_sock_fd == -1)    
    {    
		perror("socket error");    
		goto err_exit;     
    }    
    char recv_msg[BUFFER_SIZE];    
    //char input_msg[BUFFER_SIZE];    

	while(1)
	{
		if(connect(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == 0)    
		{
			fd_set client_fd_set;    
			struct timeval tv;    
		
			while(1)    
			{    
				tv.tv_sec = 20;    
				tv.tv_usec = 0;    
				FD_ZERO(&client_fd_set);    
				FD_SET(STDIN_FILENO, &client_fd_set);    
				FD_SET(server_sock_fd, &client_fd_set);    
			
				select(server_sock_fd + 1, &client_fd_set, NULL, NULL, &tv);    	//阻塞等待数据
				//if(FD_ISSET(STDIN_FILENO, &client_fd_set))    
				//{    
				//	bzero(input_msg, BUFFER_SIZE);    
				//	fgets(input_msg, BUFFER_SIZE, stdin);    
				//	if(send(server_sock_fd, input_msg, BUFFER_SIZE, 0) == -1)    
				//	{    
				//		perror("send data error!\n");    
				//	}    
				//}    
				if(FD_ISSET(server_sock_fd, &client_fd_set))    					//接收服务器发送的数据
				{    
					bzero(recv_msg, BUFFER_SIZE); 
					memset(recvkeyvalue.name,0,sizeof(recvkeyvalue.name));
					long byte_num = recv(server_sock_fd, recv_msg, BUFFER_SIZE, 0);    
					if(byte_num > 0)    
					{    
						if(byte_num > BUFFER_SIZE)    
						{    
							byte_num = BUFFER_SIZE;
						} 

						//if((recv_msg[0] == 0x53) && (recv_msg[1] == 0x35)){
						//	memcpy(&recmarkdata,recv_msg+2,sizeof(struct MarkMem));
						//	MarkMemFlag = 0;
						//	//printMark(&recmarkdata);
						if(strncmp(recv_msg, "AcStatus" ,8) == 0){
							memcpy(&recmarkdata,recv_msg+8,sizeof(struct MarkMem));
							MarkMemFlag = 0;
						}
						else{					
							memcpy(recvkeyvalue.name,recv_msg,byte_num-1);
							recvkeyvalue.KeyValue = recv_msg[byte_num-1];
							KeyProcessPoin(&recvkeyvalue);
						}
					}
					else if(byte_num < 0)    
					{    
						printf("recv data error!\n");    
					}    
					else    
					{    
						printf("server quit!\n");    
						goto err_exit;     
					}    
				}    
			}       
		}
		usleep(10000);	//等待server运行
	}		
err_exit:  
	pthread_exit((void *)-1);    
} 

int ClientInit(void)
{
    /* creat pthread */
    if ((pthread_create(&tidpclient, NULL, SocketClientPthread, NULL)) == -1)
    {
        printf("create pthread error!\n");
        return  -1; 
    }
	
	return 0;
}

void RegisterKeyFun(int (*KeyProcesstemp)(struct RecvKey *))
{
	KeyProcessPoin = KeyProcesstemp;
}


int SendDataToserver(char *inputmsgtmp, int serdata)
{
	int cnt;

	//if(serdata == 0){
	//	serdata = 100;
	//}
	
	cnt = strlen(inputmsgtmp);
	if(serdata == 0){
		sprintf(&inputmsgtmp[cnt], "%s", ":false");
		inputmsgtmp[cnt+6] = '\0';
	}
	else{
		sprintf(&inputmsgtmp[cnt], "%s", ":true");
		inputmsgtmp[cnt+5] = '\0';
	}
	//inputmsgtmp[cnt] = serdata;
	//inputmsgtmp[cnt+1] = '\0';
	
	printf("cn:%d  %s\n",cnt, inputmsgtmp);
	if(send(server_sock_fd, inputmsgtmp, strlen(inputmsgtmp), 0) == -1)    
	{    
		perror("send data error!\n");  
		return -1;
	}
	
	return 0;
}

int SetReAirLamp(int acdata)
{
	char input_msg[20]="ReAirLamp";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set ReAirLamp Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetACLamp(int acdata)
{
	char input_msg[20]="ACLamp";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set ACLamp Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetFrontLamp(int acdata)
{
	char input_msg[20]="FrontLamp";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set FrontLamp Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetRearLamp(int acdata)
{
	char input_msg[20]="RearLamp";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set RearLamp Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetACMark(int acdata)
{
	char input_msg[20]="ACMark";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set ACMark Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetAutoMark(int acdata)
{
	char input_msg[20]="AutoMark";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set AutoMark Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetHumMark(int acdata)
{
	char input_msg[20]="HumMark";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set HumMark Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetDfrMark(int acdata)
{
	char input_msg[20]="DfrMark";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set DfrMark Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetVentMark(int acdata)
{
	char input_msg[20]="VentMark";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set VentMark Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetFootMark(int acdata)
{
	char input_msg[20]="FootMark";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set FootMark Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SendDataToserverGrade(char *inputmsgtmp, int serdata)
{
	int cnt;
	
	cnt = strlen(inputmsgtmp);
	sprintf(&inputmsgtmp[cnt], ":%d", serdata);
	//inputmsgtmp[cnt] = serdata;
	inputmsgtmp[cnt+2] = '\0';
	
	printf("cn:%d  %s\n",cnt, inputmsgtmp);
	if(send(server_sock_fd, inputmsgtmp, strlen(inputmsgtmp), 0) == -1)    
	{    
		perror("send data error!\n");  
		return -1;
	}
	
	return 0;
}

int SetFanGradeMark(int acdata)
{
	char input_msg[20]="FanGradeMark";

	if(SendDataToserverGrade(input_msg, acdata) != 0){
		perror("Set FanGradeMark Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetFanMark(int acdata)
{
	char input_msg[20]="FanMark";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set FanMark Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetBackLight(int acdata)
{
	char input_msg[20]="BackLight";
	
	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set BackLight Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SetIGOFFSignal(int acdata)
{
	char input_msg[20]="IGOFFSignal";

	if(SendDataToserver(input_msg, acdata) != 0){
		perror("Set TemNum Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int SendfloatToserver(char *inputmsgtmp, float serdata)
{
	int cnt;

	//if(serdata == 0){
	//	serdata = 100;
	//}
	
	cnt = strlen(inputmsgtmp); 
	//*(float *)&inputmsgtmp[cnt] = serdata;
	sprintf(&inputmsgtmp[cnt], ":%04.1f", serdata);
	inputmsgtmp[cnt+5] = '\0';
	
	printf("cn:%d  %s\n",cnt, inputmsgtmp);
	if(send(server_sock_fd, inputmsgtmp, strlen(inputmsgtmp), 0) == -1)    
	{    
		perror("send data error!\n");  
		return -1;
	}
	
	return 0;
}

int SetTemNum(float acdata)
{
	char input_msg[15]="TemNum";

	if(SendfloatToserver(input_msg, acdata) != 0){
		perror("Set TemNum Fail!\n"); 
		return -1;
	}
	
	return 0;
}

int GetMarkStatus(struct MarkMem *markstatus)
{
	char input_msg[8]="AcStatus";
	if(send(server_sock_fd, input_msg, sizeof(input_msg), 0) == -1)    
	{    
		perror("send data error!\n");  
		return -1;
	}
	
	while(MarkMemFlag){		//等待接收server数据
		usleep(1000);
	}
	
	MarkMemFlag = 1;
	memcpy(markstatus,&recmarkdata,sizeof(struct MarkMem));

	return 0;
}









