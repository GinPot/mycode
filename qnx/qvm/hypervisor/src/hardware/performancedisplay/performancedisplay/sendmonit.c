#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "sendmonit.h"


#define PORTNUM	8747


void* pthreadgetmonit(void* arg)
{
	int on=1;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
	char sendbuf[10];

    if (sockfd == -1)
    {
        perror("socket"),exit(-1);
    }
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );	//1表示允许地址重用
	
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORTNUM);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int res = bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));
    if (res == -1)
    {
        perror("bind");
        exit(-1);
    }
    printf("getmonit bind ok\n");

    listen(sockfd,100);//监听

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int fd = accept(sockfd,(struct sockaddr*)&client,&len);//阻塞函数,等待连接
	printf("getmonit accept success\n");
	getmonitflag = 1;
	
    while (1)
    {
		if(getmonitflag == 0){
			res = recv(fd,sendbuf,sizeof(sendbuf), 0);
			if(res < 0){
				getmonitflag = 0;
				printf(" recv error\n");
				close(fd);
				fd = accept(sockfd,(struct sockaddr*)&client,&len);
				getmonitflag = 1;
			}else{
				//printf(" recv :%s\n",sendbuf);
				res = send(fd,&tomonit,sizeof(struct Todataapp), 0);
				if(res < 0){
					printf(" send error\n");
				}else{
					//printf(" send success\n");
					getmonitflag = 1;
				}
			}	
		}

		usleep(200000);
    }
    close(fd);
	
	return 0;
}