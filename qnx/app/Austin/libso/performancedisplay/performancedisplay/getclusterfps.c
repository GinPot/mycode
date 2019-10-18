#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "getclusterfps.h"

#define PORTNUM	54268


void* pthreadgetfps(void* arg)
{
#if 0
	int on=1;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
	char sendbuf[3];
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
    printf("getfps bind ok\n");

    listen(sockfd,100);//监听

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int fd = accept(sockfd,(struct sockaddr*)&client,&len);//阻塞函数,等待连接
	printf("getfps accept success\n");
	
    while (1)
    {
		res = recv(fd,sendbuf,sizeof(sendbuf), 0);
		if(res < 0){
			close(fd);
			fd = accept(sockfd,(struct sockaddr*)&client,&len);
		}else{
			//printf("~~~~~~~%x %x %d  cn:%d\n", sendbuf[0],sendbuf[1],sendbuf[2],res);
			if((sendbuf[0] == 0x89) && (sendbuf[1] == 0x98)){
				clusterfps = sendbuf[2];
				clusterfpsflag = 1;
				//printf("clusterfps = %d\n", clusterfps);
				send(fd,"suc",3, 0);
			}
		}
    }
    close(fd);
	
	return 0;
#endif

	char sendbuf[3];
	int res;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd == -1)
    {
        perror("socket");exit(-1);
    }
	
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORTNUM);//连接端口
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    while (connect(sockfd,(struct sockaddr*)&addr,sizeof(addr)) == -1) 
    {
		close(sockfd);
		sleep(1);				//等待sever上线
		
		sockfd = socket(AF_INET,SOCK_STREAM,0);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(PORTNUM);
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");		
    }
	printf("getfps connect success\n");
	
    while (1)
    {
		res = recv(sockfd,sendbuf,sizeof(sendbuf), 0);
		if(res < 0){
			printf("getfps recv error\n");
		}else{
			//printf("~~~~~~~%x %x %d  cn:%d\n", sendbuf[0],sendbuf[1],sendbuf[2],res);
			if((sendbuf[0] == 0x89) && (sendbuf[1] == 0x98)){
				clusterfps = sendbuf[2];
				clusterfpsflag = 1;
				//printf("clusterfps = %d\n", clusterfps);
				//send(sockfd,"suc",3, 0);
			}
		}
    }
    close(sockfd);
	
	return 0;	

}