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
#include "showmem.h"

extern void init_cpusload(void);
extern int Getcpuload(float *cpuload);
extern int Getmemoryload(int *memoryload);
extern int PVRSInit(void);
extern int PVRload(float *gpuload);
extern void TopPro(void);

#define PULSE_CODE_NOTIFY   0
#define BUFF_SIZE           0x1000

int channel;
char *aglbuffs;
char *shmdata;
struct hyp_shm *hsp_perform;

struct TopProceagl {
	char name[25];
	int total;
};
struct AglBuff {
	int cpu_load;
	int memoryload[2];
	struct TopProceagl appdata[5];
};
struct AglBuff *getagldata;


void shmeminit(void)
{
    channel = ChannelCreate_r(0);
    if (channel < 0) {
        printf("channel failure: %d\n", -channel);
        //goto done;
    }	
	printf("perform channel = %d\n", channel);
    //
    // Allocate the memory of the hyperivsor shared memory control structure
    //
    hsp_perform = hyp_shm_create(0);
    if (hsp_perform == NULL) {
        printf("no memory for shared memor control structure\n");
        //goto done;
    }
	
    //
    // Attach to the shared memory region. If the region status (see below)
    // changes, a pulse will be delivered to the channel 'channel' with
    // a code of 'PULSE_CODE_NOTIFY'. A pointer can be passed in the
    // pulse value field as well, though we're not using it here.
    //
    int err = hyp_shm_attach(hsp_perform, "testing", HYP_SHM_MAX_CLIENTS,
            channel, -1, PULSE_CODE_NOTIFY, NULL);
    if (err != EOK) {
        printf("shmem create failure: %d\n", err);
        //goto fail;
    }	

    //
    // The first client that attempts to create the shared memory name
    // gets to choose the actual size of the shared memory region. Since you
    // can't tell if you were the first or not, you need to check the 'size'
    // field after the creation - it will have the actual number of pages
    // of the region. This may be larger, smaller, or the same size as what
    // you requested.
    //
    unsigned const size = hyp_shm_size(hsp_perform);
    if (size != HYP_SHM_MAX_CLIENTS) {
        printf("unexpected shared memory size (%d != %d)\n", size, HYP_SHM_MAX_CLIENTS);
        //goto fail;
    }
	
    //
    // Each client that connects to shared memory region gets a unique
    // connection index number (0 through 15). This index is to form various
    // bitsets. The hyp_shm_idx() API tells you what your connection index
    // number is.
    //
    unsigned myidx = hyp_shm_idx(hsp_perform);
    printf("perform shared memory index %u\n", myidx);
	
    //
    // The hyp_shm_data() API returns a pointer to the start of the
    // shared memory region data
    //
    shmdata = hyp_shm_data(hsp_perform);
	
	aglbuffs = &shmdata[myidx*BUFF_SIZE];
	getagldata = (struct AglBuff *)aglbuffs;
	
	aglbuffs[0] = 0x45;		//The first two bytes of validation
	aglbuffs[1] = 0x54;	
	
	hyp_shm_poke(hsp_perform, HYP_SHM_POKE_ALL_BUT_ME);
}


void* TopProFun(void* arg) {
	TopPro();
	return 0;
}

int main(int argc, char *argv[]) 
{
	int i,ret;
	float cpuload[7];
	int memoryload[]={4096,1280,0,2816,0};
	float gpuload[4];
	
	//sleep(15);
	
	shmeminit();
	init_cpusload();
	ret = PVRSInit();
	if(ret != 0)
		printf("Error initializing PVRScope.\n");

	ret = pthread_create(NULL, NULL, TopProFun, NULL);
	if (ret)
		perror("pthread_create TopPro failed");

	while(1)
	{
		Getcpuload(cpuload);
		ret = Getmemoryload(memoryload);
		if(ret != 0)
			printf("Get memory load fail!\n");
		PVRload(gpuload);
		
		
		
		printf("\n》》》\n");
		printf("CPU load：\n");
		printf("	CPU states: %.1f%\n",cpuload[0]);
		printf("	AGL vCPU states: %.1f%\n",(float)getagldata->cpu_load/10);	
	
        printf("\n");
		printf("Memory load：\n");
		printf("	Board total:%dM\n",memoryload[0]);
		printf("	QNX total  :%dM\n",memoryload[1]);
		printf("	QNX avail  :%dM\n\n",memoryload[2]);
		printf("	AGL total  :%dM\n",getagldata->memoryload[0]);
		printf("	AGL avail  :%dM\n",getagldata->memoryload[1]);
        
		printf("\n");
		printf("GPU load：\n");
		printf("	Frames per second       :%.1fFPS\n",gpuload[0]);
		printf("	Tiler active            :%.1f%\n",gpuload[1]);
		printf("	Shader processing load  :%.1f%\n",gpuload[2]);
		printf("	Renderer active         :%.1f%\n",gpuload[3]);
		
		printf("\n");
		printf("QNX process：\n");
		for(i=0; i<5; i++)
			printf("	%.1fM	app:%s\n", (float)procedata[i].total/1048576, procedata[i].name);
		printf("AGL process：\n");
		for(i=0; i<5; i++)
			printf("	%.1fM	app:%s\n", (float)getagldata->appdata[i].total/10, getagldata->appdata[i].name);
		
		delay(3000);		
	}
    return 0;
}
