#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <pthread.h>

#include "lin_aurt.h" 
#include "socket_server.h"

#define BACKLOG 5     		//完成三次握手但没有accept的队列的长度    
#define CONCURRENT_MAX 8    //应用层同时可以处理的连接    
#define SERVER_PORT 11332    
#define BUFFER_SIZE 1024 
#define QUIT_CMD ".quit"   


extern struct MarkMem MarkMemValue;
int client_fds[CONCURRENT_MAX];  
fd_set server_fd_set; 	//fd_set 



void printMark(struct MarkMem *MarkMemValuetmp)
{
	printf("MarkMemValue.ReAirLamp=%d\n",MarkMemValuetmp->ReAirLamp);
	printf("MarkMemValue.ACLamp=%d\n",MarkMemValuetmp->ACLamp);
	printf("MarkMemValue.FrontLamp=%d\n",MarkMemValuetmp->FrontLamp);
	printf("MarkMemValue.RearLamp=%d\n",MarkMemValuetmp->RearLamp);
	printf("MarkMemValue.ACMark=%d\n",MarkMemValuetmp->ACMark);
	printf("MarkMemValue.AutoMark=%d\n",MarkMemValuetmp->AutoMark);
	printf("MarkMemValue.HumMark=%d\n",MarkMemValuetmp->HumMark);
	printf("MarkMemValue.DfrMark=%d\n",MarkMemValuetmp->DfrMark);
	printf("MarkMemValue.VentMark=%d\n",MarkMemValuetmp->VentMark);
	printf("MarkMemValue.FootMark=%d\n",MarkMemValuetmp->FootMark);
	printf("MarkMemValue.FanGradeMark=%d\n",MarkMemValuetmp->FanGradeMark);
	printf("MarkMemValue.FanMark=%d\n",MarkMemValuetmp->FanMark);
	printf("MarkMemValue.BackLight=%d\n",MarkMemValuetmp->BackLight);
	printf("MarkMemValue.IGOFFSignal=%d\n",MarkMemValuetmp->IGOFFSignal);
	printf("MarkMemValue.TemNum=%.1f\n",MarkMemValuetmp->TemNum);
}


int SendMarkStatus(void)
{
	int i;
	char senddatamark[68]="AcStatus";
	struct MarkMem *MarkMemValuetmp123;
	
	
	memcpy(senddatamark+8,&MarkMemValue,sizeof(struct MarkMem));
	
	//MarkMemValuetmp123 = (struct MarkMem *)&senddatamark[2];
	//printMark(MarkMemValuetmp123);
	
    for(int i = 0; i < CONCURRENT_MAX; i++)    
    {    
        if(client_fds[i] != 0)    
        {    
			send(client_fds[i], senddatamark, sizeof(senddatamark), 0);    
        }    
    }
	
	return 0;
}

void *SocketServerPthread(void *arg)
{
	int on=1;
	char *pNext;
	struct RecvKey recvclientvalue;
	char input_msg[BUFFER_SIZE];    
	char recv_msg[BUFFER_SIZE]; 
    //本地地址    
    struct sockaddr_in server_addr;    
    server_addr.sin_family = AF_INET;    
    server_addr.sin_port = htons(SERVER_PORT);    
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");    
    bzero(&(server_addr.sin_zero), 8);    
    //创建socket    
    int server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);    
    if(server_sock_fd == -1)    
    {    
        perror("socket error");    
        goto err_exit;   
    }    
	setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );	//1表示允许地址重用
    //绑定socket    
    int bind_result = bind(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(bind_result == -1)    
    {    
        perror("bind error");    
        goto err_exit;    
    }    
    //listen    
    if(listen(server_sock_fd, BACKLOG) == -1)    
    {    
        perror("listen error");    
        goto err_exit;      
    }    
	
	//fd_set server_fd_set; 	//fd_set 
    int max_fd = -1;    
    struct timeval tv;  //超时时间设置    
	tv.tv_sec = 0;    
    tv.tv_usec = 0;  
    while(1)    
    {    
		//printf("~~~~~~~~~~~~~~~\n"); 
		//usleep(10000);  
		tv.tv_sec = 10;    
		tv.tv_usec = 0; 
        FD_ZERO(&server_fd_set);    
        //FD_SET(STDIN_FILENO, &server_fd_set);    
        //if(max_fd <STDIN_FILENO)    
        //{    
         //   max_fd = STDIN_FILENO;    
        //}    
        //printf("STDIN_FILENO=%d\n", STDIN_FILENO);    
		//服务器端socket    
        FD_SET(server_sock_fd, &server_fd_set);    
       // printf("server_sock_fd=%d\n", server_sock_fd);    
        if(max_fd < server_sock_fd)    
        {    
            max_fd = server_sock_fd;    
        }    
		//客户端连接    
        for(int i =0; i < CONCURRENT_MAX; i++)    
        {    
            //printf("client_fds[%d]=%d\n", i, client_fds[i]);    
            if(client_fds[i] != 0)    
            {    
                FD_SET(client_fds[i], &server_fd_set);    
                if(max_fd < client_fds[i])    
                {    
                    max_fd = client_fds[i];    
                }    
            }    
        }    
        int ret = select(max_fd + 1, &server_fd_set, NULL, NULL, &tv);    
        if(ret < 0)    
        {    
            perror("select error\n");    
            continue;    
        }    
        else if(ret == 0)    
        {    
            //printf("select timeout\n");    
            continue;    
        }    
        else    
        {    
            //ret 为未状态发生变化的文件描述符的个数    
            //if(FD_ISSET(STDIN_FILENO, &server_fd_set))    
            //{    
            //    printf("send data:\n");    
            //    bzero(input_msg, BUFFER_SIZE);    
            //    fgets(input_msg, BUFFER_SIZE, stdin);    
            //    //输入“.quit"则退出服务器    
            //    if(strcmp(input_msg, QUIT_CMD) == 0)    
            //    {    
            //        exit(0);    
            //    }    
            //    for(int i = 0; i < CONCURRENT_MAX; i++)    
            //    {    
            //        if(client_fds[i] != 0)    
            //        {    
            //            printf("client_fds[%d]=%d\n", i, client_fds[i]);    
            //            send(client_fds[i], input_msg, BUFFER_SIZE, 0);    
            //        }    
            //    }    
            //}    
            if(FD_ISSET(server_sock_fd, &server_fd_set))    
            {    
                //有新的连接请求    
                struct sockaddr_in client_address;    
                socklen_t address_len;    
                int client_sock_fd = accept(server_sock_fd, (struct sockaddr *)&client_address, &address_len);    
                printf("new connection client_sock_fd = %d\n", client_sock_fd);    
                if(client_sock_fd > 0)
                {    
                    int index = -1;    
                    for(int i = 0; i < CONCURRENT_MAX; i++)    
                    {    
                        if(client_fds[i] == 0)    
                        {    
                            index = i;    
                            client_fds[i] = client_sock_fd;    
                            break;    
                        }    
                    }    
                    if(index >= 0)    
                    {    
                        printf("new clent(%d)add success %s:%d\n", index, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));    
                    }    
                    else    
                    {    
                        bzero(input_msg, BUFFER_SIZE);    
                        strcpy(input_msg, "Server can't join because the number of clients joined reaches the maximum!\n");    
                        send(client_sock_fd, input_msg, BUFFER_SIZE, 0);    
                        printf("The number of client connections reached the maximum, and the new client failed to join %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));    
                    }    
                }    
            }    
            for(int i =0; i < CONCURRENT_MAX; i++)    
            {    
                if(client_fds[i] !=0)    
                {    
                    if(FD_ISSET(client_fds[i], &server_fd_set))    
                    {    
                        //处理某个客户端过来的消息    
                        bzero(recv_msg, BUFFER_SIZE);    
						memset(recvclientvalue.name,0,sizeof(recvclientvalue.name));
                        long byte_num = recv(client_fds[i], recv_msg, BUFFER_SIZE, 0);    
                        if (byte_num > 0)
                        {    
                            if(byte_num > BUFFER_SIZE)    
                            {    
                                byte_num = BUFFER_SIZE;    
                            }    
                            //recv_msg[byte_num] = '\0';    
                            //printf("~~~~~~~Clent(%d):%s  cn:%d\n", i, recv_msg,byte_num);
							

							char *p = &recv_msg[0];
							while(p < recv_msg+byte_num)
							{
								int nlen = strlen(p) + 1;
								
								if(strncmp(p, "AcStatus" ,8) == 0){
									SendMarkStatus();
								}else
								{

									//printf("!!!!!!!!p:%s  byte_num=%d\n", p,byte_num);
									pNext = strtok(p, ":");
									memcpy(recvclientvalue.name, pNext, strlen(pNext)+1); 

									//printf("%s  %s\n", pNext,recvclientvalue.name); 
									
									pNext = strtok(NULL,":");

									//printf("%s\n", pNext); 
									if(strcmp(pNext, "false") == 0){
										recvclientvalue.KeyValue = 0;
									}else{
										if(strcmp(pNext, "true") == 0){
											recvclientvalue.KeyValue = 1;
										}
										else{
											recvclientvalue.KeyValue = atof(pNext);
										}
									}

									ClientProcess(&recvclientvalue);
									//printf("Clent(%d):%s=%.1f\n", i, recvclientvalue.name,recvclientvalue.KeyValue);  
								}
								
								p += nlen;
							}
                        }    
                        else if(byte_num < 0)    
                        {    
                            printf("Clent(%d)Error receiving message.\n", i);    
                        }    
                        else    
                        {    
                            FD_CLR(client_fds[i], &server_fd_set);    
                            client_fds[i] = 0;    
                            printf("Clent(%d)quit\n", i);    
                        }    
                    }    
                }    
            }    
        }
    }  	
err_exit:  
	pthread_exit((void *)-1); 	
}

void SocketServerSend(char *keyname, int keyvalue)
{
	char input_msg[15]; 
	int cnt;
    
	cnt = strlen(keyname); 
	memcpy(input_msg,keyname,cnt);
	input_msg[cnt] = keyvalue;
	input_msg[cnt+1] = '\0';
  
    for(int i = 0; i < CONCURRENT_MAX; i++)    
    {    
        if(client_fds[i] != 0)    
        {    
            printf("client_fds[%d]=%d   %s=%d  cnt=%d\n", i, client_fds[i],input_msg,input_msg[cnt],strlen(input_msg));    
            send(client_fds[i], input_msg, strlen(input_msg), 0);    
        }    
    }    
}


int ClientProcess(struct RecvKey *recvClienttmp)			
{
	if(strcmp(recvClienttmp->name, "ReAirLamp") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.ReAirLamp = recvClienttmp->KeyValue;
		return 0;
	}
	if(strcmp(recvClienttmp->name, "ACLamp") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.ACLamp = recvClienttmp->KeyValue;
		return 0;
	}
	if(strcmp(recvClienttmp->name, "FrontLamp") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.FrontLamp = recvClienttmp->KeyValue;
		return 0;
	}
	if(strcmp(recvClienttmp->name, "RearLamp") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.RearLamp = recvClienttmp->KeyValue;
		return 0;
	}
	
	
	if(strcmp(recvClienttmp->name, "ACMark") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.ACMark = recvClienttmp->KeyValue;
		return 0;
	}
	if(strcmp(recvClienttmp->name, "AutoMark") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.AutoMark = recvClienttmp->KeyValue;
		return 0;
	}
	if(strcmp(recvClienttmp->name, "HumMark") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.HumMark = recvClienttmp->KeyValue;
		return 0;
	}
	if(strcmp(recvClienttmp->name, "DfrMark") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.DfrMark = recvClienttmp->KeyValue;
		return 0;
	}
	if(strcmp(recvClienttmp->name, "VentMark") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.VentMark = recvClienttmp->KeyValue;
		return 0;
	}
	if(strcmp(recvClienttmp->name, "FootMark") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.FootMark = recvClienttmp->KeyValue;
		return 0;
	}
	
	
	if(strcmp(recvClienttmp->name, "FanGradeMark") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.FanGradeMark = recvClienttmp->KeyValue;
		return 0;
	}
	if(strcmp(recvClienttmp->name, "FanMark") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.FanMark = recvClienttmp->KeyValue;
		return 0;
	}
	
	
	if(strcmp(recvClienttmp->name, "BackLight") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.BackLight = recvClienttmp->KeyValue;
		return 0;
	}
	if(strcmp(recvClienttmp->name, "IGOFFSignal") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.IGOFFSignal = recvClienttmp->KeyValue;
		return 0;
	}
	if(strcmp(recvClienttmp->name, "TemNum") == 0){
		printf("%s=%.1f\n", recvClienttmp->name, recvClienttmp->KeyValue); 
		MarkMemValue.TemNum = recvClienttmp->KeyValue;
		return 0;
	}
}
