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

#define ATTACH_POINT "getmonit"

#define PORTNUM 8847

struct ToProceapp {
	int pid;
	float cpuload;
	float memoryload;
	char name[15];
};
struct BusMoniMemb {
	char name[35];
	int rdata;
	char rdatacom[4];
	int wdata;
	char wdatacom[4];
};
struct Todataapp {
	float qnxcpu[7];
	float aglcpu[5];
	
	int totalmemory;
	int qnxmemory;
	int qnxmemoryavail;
	int aglmemory;
	int aglmemoryavail;

	int clusterfps;
	int ivifps;
	int alexafps;
	float gpufps;
	float gputiler;
	float gpushader;
	float gpurenderer;
	
	struct ToProceapp toqnxapp[7];
	struct ToProceapp toaglapp[6];
	struct BusMoniMemb busmonidata[10];
};
struct Todataapp toclusterapp;


int main(int argc, char *argv[])
{
	int res,i;
	char buf[3]={0xAB,0xBA},recvbuff[3];

	if(argc != 2){
		printf("Invalid parameter:\n");
		printf("  getmonit [select]\n");
		printf("  [select]: vga or log or all\n");
		return 0;
	}

	if(!strncmp(argv[1], "vga", 3)){
		buf[2] = 1;
	}
	else{
		if(!strncmp(argv[1], "log", 3)){
			buf[2] = 2;
		}
		else{
			if(!strncmp(argv[1], "all", 3)){
				buf[2] = 3;
			}
			else{
				printf("Invalid parameter:\n");
				printf("  getmonit [select]\n");
				printf("  [select]: vga or log or all\n");
				return 0;
			}
		}
	}
	//printf("parameter:%s\n",argv[1]);
	
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
    //printf("Successful connection,send commond.\n");
	
	res = send(sockfd,buf,sizeof(buf), 0);
	if(res < 0){
        printf("getmonit: send error\n");
		return 0;
	}
    res = recv(sockfd, &recvbuff, sizeof(recvbuff), 0);
    if (res < 0) {
        printf("getmonit: recv suc error\n");
		return 0;
    }
	if(strncmp(recvbuff, "suc", 3)){
        printf("getmonit: recvbuff not suc\n");
		return 0;
	}
	//printf("commond send suc\n");
	
	while(1)
	{
		if((buf[2] == 1)){
			res = send(sockfd,buf,sizeof(buf), 0);
			if(res < 0){
				printf("getmonit: send error\n");
				return 0;
			}
			sleep(1);
		}
		
		if((buf[2] == 2) || (buf[2] == 3)){
			res = send(sockfd,buf,sizeof(buf), 0);
			if(res < 0){
				printf("getmonit: send error\n");
				return 0;
			}
			res = recv(sockfd, &toclusterapp, sizeof(struct Todataapp), 0);
			if (res < 0) {
				printf("getmonit: recv error\n");
				return 0;
			}

			printf("\n>>>\n");
			printf("CPU load:\n");
			printf("	CPU total:	%.1f%\n",toclusterapp.qnxcpu[0]);
			printf("	CPU[1]:%.1f%	CPU[2]:%.1f%	CPU[3]:%.1f%	CPU[4]:%.1f%	CPU[5]:%.1f%	CPU[6]:%.1f%\n",toclusterapp.qnxcpu[1],toclusterapp.qnxcpu[2],toclusterapp.qnxcpu[3],toclusterapp.qnxcpu[4],toclusterapp.qnxcpu[5],toclusterapp.qnxcpu[6]);

			printf("\n");
			printf("	AGL vCPU total:	%.1f%\n",toclusterapp.aglcpu[0]);
			printf("	vCPU[1]:%.1f%	vCPU[2]:%.1f%	vCPU[3]:%.1f%	vCPU[4]:%.1f%\n",toclusterapp.aglcpu[1],toclusterapp.aglcpu[2],toclusterapp.aglcpu[3],toclusterapp.aglcpu[4]);	
			
			printf("\n");
			printf("Memory load:\n");
			printf("	Board total: %dM\n",toclusterapp.totalmemory);
			printf("	QNX   : %dM / %dM\n",toclusterapp.qnxmemoryavail,toclusterapp.qnxmemory);
			printf("	AGL   : %dM / %dM\n",toclusterapp.aglmemoryavail,toclusterapp.aglmemory);
			
			printf("\n");
			printf("GPU load:\n");
			printf("	Frames per second       : %.1fFPS   AGL:%dFPS  QNX:%dFPS\n",toclusterapp.gpufps,toclusterapp.ivifps,toclusterapp.clusterfps);
			printf("	Tiler active            : %.1f%\n",toclusterapp.gputiler);
			printf("	Shader processing load  : %.1f%\n",toclusterapp.gpushader);
			printf("	Renderer active         : %.1f%\n",toclusterapp.gpurenderer);
			
			printf("\n");
			printf("QNX process:\n");
			printf("	PID         CPU         MEM         COMMAND\n");
			for(i=0; i<5; i++)
				printf("	%d       %.1f       %.1fMB       %s\n", toclusterapp.toqnxapp[i].pid, toclusterapp.toqnxapp[i].cpuload, toclusterapp.toqnxapp[i].memoryload, toclusterapp.toqnxapp[i].name);
			
			printf("\n");
			printf("AGL process:\n");
			printf("	PID         CPU         MEM         COMMAND\n");
			for(i=0; i<5; i++)            
				printf("	%d       %.1f       %.1fMB       %s\n", toclusterapp.toaglapp[i].pid, toclusterapp.toaglapp[i].cpuload, toclusterapp.toaglapp[i].memoryload, toclusterapp.toaglapp[i].name);
			
			printf("\n");
			printf("Bus Monitoring:\n");
			printf("	R                W                IP\n");
			for(i=0; i<10; i++)            
				printf("	%d %s		%d %s   	%s \n", toclusterapp.busmonidata[i].rdata, toclusterapp.busmonidata[i].rdatacom,toclusterapp.busmonidata[i].wdata, toclusterapp.busmonidata[i].wdatacom,toclusterapp.busmonidata[i].name);
			
			sleep(1);
		}
	}	

    return 0;
}
