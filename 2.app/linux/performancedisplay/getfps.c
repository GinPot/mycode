#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <pthread.h>

#include "getfps.h"
#include "topprocess.h"

#define BACKLOG 5     		//完成三次握手但没有accept的队列的长度    
#define CONCURRENT_MAX 8    //应用层同时可以处理的连接  
#define PORTNUM	25465
#define BUFFER_SIZE	100


struct AglBuffda {
	int ivifps;
	int alexafps;
	float aglcpuload[5];
	float aglmemoryload[2];
	struct TopProce aglappdata[PROCESSNUM];
};
extern struct AglBuffda *agldatabu;

void *GetFpsPthread(void *arg)
{
	int on=1;
	char *pNext;
	char input_msg[BUFFER_SIZE];    
	char recv_msg[BUFFER_SIZE]; 
	int client_fds[CONCURRENT_MAX];  
    //本地地址    
    struct sockaddr_in server_addr;    
    server_addr.sin_family = AF_INET;    
    server_addr.sin_port = htons(PORTNUM);    
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
	
	fd_set server_fd_set; 	//fd_set 
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
                        long byte_num = recv(client_fds[i], recv_msg, BUFFER_SIZE, 0);    
                        if (byte_num > 0)
                        {    
                            if(byte_num > BUFFER_SIZE)    
                            {    
                                byte_num = BUFFER_SIZE;    
                            }    
                            //recv_msg[byte_num] = '\0';    
                            printf("~~~~~~~Clent(%d):%x %x %d  cn:%d\n", i, recv_msg[0],recv_msg[1],recv_msg[2],byte_num);
							if((recv_msg[0] == 0x23) && (recv_msg[1] == 0x32)){
								agldatabu->alexafps = recv_msg[2];
								printf("agldatabu->alexafps = %d\n", agldatabu->alexafps);
							}
							if((recv_msg[0] == 0x67) && (recv_msg[1] == 0x76)){
								agldatabu->ivifps = recv_msg[2];
								printf("agldatabu->ivifps = %d\n", agldatabu->ivifps);
							}
							send(client_fds[i],"suc",3, 0);
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