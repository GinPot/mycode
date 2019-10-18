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


#define PORTNUM	54269


int main(int argc, char *argv[])
{
	int res;
	char buf[3];
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
    printf("连接成功\n");

    while (1)
    {
		//buf[0] = 0x89;			//两个字节的头，0x89、0x98为cluster；
		//buf[1] = 0x98;
        //scanf("%d",&buf[2]);	//帧率
		//
        //send(sockfd,buf,strlen(buf), 0);

        res = recv(sockfd,buf,sizeof(buf), 0);
        printf("读到了%d字节，内容：%x %x %x\n",res,buf[0],buf[1],buf[2]);	//读到“suc”字符表示正常
		
		//sleep(1);
    }

    close(sockfd);
    return 0;
}
