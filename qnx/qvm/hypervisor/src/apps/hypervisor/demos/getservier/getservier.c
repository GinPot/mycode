#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>


#define PORTNUM	54268


int main(int argc, char *argv[])
{
	int on=1;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
	char sendbuf[3]={0x89,0x98,0};
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
    printf("getservier bind ok\n");

    listen(sockfd,100);//监听

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int fd = accept(sockfd,(struct sockaddr*)&client,&len);//阻塞函数,等待连接
	printf("getservier accept success\n");
	
    while (1)
    {

		send(fd,sendbuf,3, 0);
		printf("send sendbuf[2]:%x %x %d\n",sendbuf[0],sendbuf[1],sendbuf[2]);
		sendbuf[2]++;
		
		sleep(1);
    }
    close(fd);
	
	return 0;
}