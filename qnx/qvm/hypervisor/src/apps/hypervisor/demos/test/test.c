#include <sys/iomgr.h>
#include <sys/siginfo.h>
#include <hyp_shm.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <screen/screen.h>
#include <sys/dispatch.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <sys/time.h>

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

struct timeval time1;
struct timeval time2;

void (*Initperformapp)(void);
void (*Getperformapp)(struct Todataapp *);

void outPutFpsInfo(void) {
	static int fs_numb=0;
	struct timeval endtime;
	static double timeuse, old_timeuse;
	
	fs_numb++;
	gettimeofday(&endtime,0);
	timeuse = 1000000*(endtime.tv_sec) + endtime.tv_usec;
	timeuse = timeuse/1000000;	
	if(timeuse > old_timeuse+1){
		old_timeuse = timeuse;
		
		printf("timeuse = %f   fs_numb = %d fs\n",timeuse,fs_numb);
		
		fs_numb = 0;
	}
}

int main(int argc, char *argv[])
{
	int i;

	void *handle = dlopen( "/root/lib-performancedisplay.so", RTLD_LOCAL );	///usr/lib/lib-performancedisplay.so
	if( NULL == (Initperformapp = dlsym( handle, "initperform" )) ) 
	{ 
		printf( "unresolved symbol '" "CameraOn" "' in '%s'\n", "/lib-camera_lib.so" );
		dlclose(handle); 
		return -1; 
	}
	if( NULL == (Getperformapp = dlsym( handle, "getperform" )) ) 
	{ 
		printf( "unresolved symbol '" "CameraOffapp" "' in '%s'\n", "/lib-camera_lib.so" );
		dlclose(handle); 
		return -1; 
	}
	Initperformapp();

	
	
	while(1)
	{
		Getperformapp(&toclusterapp);

		//printf("\n>>>\n");
		//printf("CPU load:\n");
		//printf("	CPU states:	 %.1f%\n",toclusterapp.qnxcpu[0]);
		//for(i=1; i<7; i++){
		//	printf("	  CPU[%d] states:	 %.1f%\n",i,toclusterapp.qnxcpu[i]);
		//}
		//printf("\n");
		//printf("	AGL vCPU states:	 %.1f%\n",toclusterapp.aglcpu[0]);
		//for(i=1; i<5; i++){
		//	printf("	  AGL vCPU[%d] states:	 %.1f%\n",i,toclusterapp.aglcpu[i]);
		//}	
		//
		//printf("\n");
		//printf("Memory load:\n");
		//printf("	Board total: %dM\n",toclusterapp.totalmemory);
		//printf("	QNX total  : %dM\n",toclusterapp.qnxmemory);
		//printf("	QNX avail  : %dM\n\n",toclusterapp.qnxmemoryavail);
		//printf("	AGL total  : %dM\n",toclusterapp.aglmemory);
		//printf("	AGL avail  : %dM\n",toclusterapp.aglmemoryavail);
		//
		//printf("\n");
		//printf("GPU load:\n");
		//printf("	Frames per second       : %.1fFPS   Alexa:%dFPS  IVI:%dFPS  Cluster:%dFPS\n",toclusterapp.gpufps,toclusterapp.alexafps,toclusterapp.ivifps,toclusterapp.clusterfps);
		//printf("	Tiler active            : %.1f%\n",toclusterapp.gputiler);
		//printf("	Shader processing load  : %.1f%\n",toclusterapp.gpushader);
		//printf("	Renderer active         : %.1f%\n",toclusterapp.gpurenderer);
		//
		//printf("\n");
		//printf("QNX process:\n");
		//for(i=0; i<6; i++)
		//	printf("%d       %.1f       %.1fMB       %s\n", toclusterapp.toqnxapp[i].pid, toclusterapp.toqnxapp[i].cpuload, toclusterapp.toqnxapp[i].memoryload, toclusterapp.toqnxapp[i].name);
		//
		//printf("\n");
		//printf("AGL process:\n");    
		//for(i=0; i<6; i++)            
		//	printf("%d       %.1f       %.1fMB       %s\n", toclusterapp.toaglapp[i].pid, toclusterapp.toaglapp[i].cpuload, toclusterapp.toaglapp[i].memoryload, toclusterapp.toaglapp[i].name);
		//
		//printf("Bus Monitoring:\n");
		//for(i=0; i<10; i++)            
		//	printf("	%d %s		%d %s   	%s \n", toclusterapp.busmonidata[i].rdata, toclusterapp.busmonidata[i].rdatacom,toclusterapp.busmonidata[i].wdata, toclusterapp.busmonidata[i].wdatacom,toclusterapp.busmonidata[i].name);
        
		//outPutFpsInfo();
		usleep(1000*1000);
	}	
	

	
    return 0;
}
