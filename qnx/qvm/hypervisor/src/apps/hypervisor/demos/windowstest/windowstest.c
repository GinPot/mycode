#include <sys/iomgr.h>
#include <sys/siginfo.h>
#include <hyp_shm.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <screen/screen.h>
#include <dlfcn.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>


char sendbuf[3];


int main(int argc, char *argv[])
{
	int on=1;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd == -1)
    {
        perror("socket"),exit(-1);
    }
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );	//1表示允许地址重用
	
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(2581);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int res = bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));
    if (res == -1)
    {
        perror("bind");
        exit(-1);
    }
    printf("bind ok\n");

    listen(sockfd,100);//监听

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int fd = accept(sockfd,(struct sockaddr*)&client,&len);//阻塞函数,等待连接
	
	
    while (1)
    {
        memset(sendbuf,0,sizeof(sendbuf));//buf清0
        scanf("%d",&sendbuf[0]);
        write(fd,sendbuf,strlen(sendbuf));

    }
    close(fd);
	
    return 0;
}
