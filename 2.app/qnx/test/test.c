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

struct ToProceapp {
	char name[35];
	float total;
};
struct Todataapp {
	float qnxcpu;
	float aglcpu;
	
	int totalmemory;
	int qnxmemory;
	int qnxmemoryavail;
	int aglmemory;
	int aglmemoryavail;
	
	float gpufps;
	float gputiler;
	float gpushader;
	float gpurenderer;
	
	struct ToProceapp toqnxapp[5];
	struct ToProceapp toaglapp[5];
};
struct Todataapp toclusterapp;


void (*Initperformapp)(void);
void (*Getperformapp)(struct Todataapp *);

int main(int argc, char *argv[]) 
{
	int i;
	
	void *handle = dlopen( "/usr/lib/lib-performancedisplay.so", RTLD_LOCAL );	
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
		
		printf("\n》》》\n");
		printf("CPU load：\n");
		printf("	CPU states:	 %.1f%\n",toclusterapp.qnxcpu);
		printf("	AGL vCPU states: %.1f%\n",toclusterapp.aglcpu);	
	    
        printf("\n");
		printf("Memory load：\n");
		printf("	Board total: %dM\n",toclusterapp.totalmemory);
		printf("	QNX total  : %dM\n",toclusterapp.qnxmemory);
		printf("	QNX avail  : %dM\n\n",toclusterapp.qnxmemoryavail);
		printf("	AGL total  : %dM\n",toclusterapp.aglmemory);
		printf("	AGL avail  : %dM\n",toclusterapp.aglmemoryavail);
        
		printf("\n");
		printf("GPU load：\n");
		printf("	Frames per second       : %.1fFPS\n",toclusterapp.gpufps);
		printf("	Tiler active            : %.1f%\n",toclusterapp.gputiler);
		printf("	Shader processing load  : %.1f%\n",toclusterapp.gpushader);
		printf("	Renderer active         : %.1f%\n",toclusterapp.gpurenderer);
		
		printf("\n");
		printf("QNX process：\n");
		for(i=0; i<5; i++)
			printf("	%.1fM	app:  %s\n", toclusterapp.toqnxapp[i].total, toclusterapp.toqnxapp[i].name);
		printf("AGL process：\n");    
		for(i=0; i<5; i++)            
			printf("	%.1fM	app:  %s\n", toclusterapp.toaglapp[i].total, toclusterapp.toaglapp[i].name);
		
		delay(3000);	
	}	
	

	
    return 0;
}
