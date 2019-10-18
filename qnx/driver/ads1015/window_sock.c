#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "window_sock.h"

#define PORTNUM	54269

static int sockfd;
static int fd;

void* pthreadwindow(void* arg)
{
	int on=1;
	widowkeyvalue[0] = 0x78;
	widowkeyvalue[1] = 0x87;
	
    sockfd = socket(AF_INET,SOCK_STREAM,0);
	
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
    printf("pthreadwindow bind ok\n");

    listen(sockfd,100);//监听

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    fd = accept(sockfd,(struct sockaddr*)&client,&len);//阻塞函数,等待连接
	printf("pthreadwindow accept success\n");
	windowsendflag = 1;
	
	
    while (1)
    {
		if(windowsendflag == 0){
			res = send(fd, widowkeyvalue, sizeof(widowkeyvalue), 0);
			if(res < 0){
				printf("pthreadwindow send error\n");
				windowsendflag = 0;
				close(fd);
				fd = accept(sockfd,(struct sockaddr*)&client,&len);
				printf("pthreadwindow accept success\n");
				windowsendflag = 1;
			}else{
				//printf("pthreadwindow send success\n");
				windowsendflag = 1;
			}
		}
		
		usleep(25000);

    }
	
    close(fd);
	return 0;
}