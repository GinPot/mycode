#include <hyp_shm.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "performancedisplay.h"
#include "cpuload.h"
#include "memoryload.h"
#include "topprocess.h"
#include "busmoni.h"
#include "getclusterfps.h"
#include "sendmonit.h"

extern int PVRSInit(void);
extern int PVRload(float *gpuload);


#define PULSE_CODE_NOTIFY   0
#define BUFF_SIZE           0x1000

static int channel;
static char *aglbuffs;
static char *shmdata;
static struct hyp_shm *hsp_perform;

struct TopProce {
	int pid;
	float cpuload;
	float memoryload;
	char name[15];
};
struct AglBuff {
	int ivifps;
	int alexafps;
	float aglcpuload[5];
	float aglmemoryload[2];
	struct TopProce aglappdata[5];
};
static struct AglBuff *getagldata=NULL;

static float gpuload[4];
static struct BusMoniMemb busdata[10];
static int gpuloadflag, busmoniflag;
int sleepflag;
struct  timezone   tz;
static struct timeval time1;
static float nowtime, oldtime;


static void shmeminit(void)
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

	sleep(1);
	hyp_shm_poke(hsp_perform, HYP_SHM_POKE_ALL_BUT_ME);
}

static void* pthreadgpuload(void* arg)
{
	sleep(20);
	shmeminit();
	
	while(1)
	{
		if(sleepflag == 0){
			PVRload(gpuload);
			gpuloadflag = 1;
		}else{
			sleep(1);
		}
	}
	
	return 0;
}

static void* pthreadbusmoni(void* arg)
{

	while(1)
	{
		if(sleepflag == 0){
			GetBusMoni((struct BusMoniMems *)busdata);
			busmoniflag = 1;
		}else{
			sleep(1);
		}
		
		gettimeofday(&time1, &tz);
		nowtime = time1.tv_sec + (float)time1.tv_usec/1000000;
		if(nowtime >= oldtime + 2){
			sleepflag = 1;
		}
	}
	
	return 0;
}

void initperform(void)
{
	int ret;

	ret = pthread_create(NULL, NULL, pthreadgpuload, NULL);
	if (ret) {
		perror("pthread_create failed");
	}
	ret = pthread_create(NULL, NULL, pthreadbusmoni, NULL);
	if (ret) {
		perror("pthread_create failed");
	}
	ret = pthread_create(NULL, NULL, pthreadgetfps, NULL);
	if (ret) {
		perror("pthread_create failed");
	}
	//ret = pthread_create(NULL, NULL, pthreadgetmonit, NULL);
	//if (ret) {
	//	perror("pthread_create failed");
	//}

	InitCpuLoad();
	ret = PVRSInit();
	if(ret != 0)
		printf("Error initializing PVRScope.\n");
}

void getperform(struct Todataapp *tocluster)
{
	int aglmemorytemp;
	float aglcpuloadtmp[2];
	
	sleepflag = 0;
	gettimeofday(&time1, &tz);
	oldtime = time1.tv_sec + (float)time1.tv_usec/1000000;
	
	GetCpuLoad(tocluster->qnxcpu);
	GetMemoryLoad(&tocluster->qnxmemoryavail);
	GetTopPro((struct ToQnxProce *)tocluster->toqnxapp);
	//PVRload(gpuload);
	//GetBusMoni(tocluster->busmonidata);
	
	tocluster->totalmemory = 4096;
	tocluster->qnxmemory = 1024;
	tocluster->aglmemory = 3072;

	if(gpuloadflag == 1){
		tocluster->gpufps = gpuload[0];
		tocluster->gputiler = gpuload[1];
		tocluster->gpushader = gpuload[2];
		tocluster->gpurenderer = gpuload[3];
		gpuloadflag = 0;
	}
	
	if(busmoniflag == 1){
		memcpy(tocluster->busmonidata, busdata, sizeof(tocluster->busmonidata));
		busmoniflag = 0;
	}
	
	if(clusterfpsflag == 1){
		tocluster->clusterfps = clusterfps;
		clusterfpsflag = 0;
	}

	if(getagldata == NULL){
		memset(tocluster->toaglapp, 0, sizeof(tocluster->toaglapp));
		memset(tocluster->aglcpu, 0, sizeof(tocluster->aglcpu));
		tocluster->aglmemoryavail = 0;
	}else{
		aglmemorytemp = getagldata->aglmemoryload[1];
		aglcpuloadtmp[0] = getagldata->aglcpuload[0];
		aglcpuloadtmp[1] = getagldata->aglappdata[0].cpuload;
		
		if((aglmemorytemp != 0) && (aglcpuloadtmp[0] != 0) && (aglcpuloadtmp[1] != 0)){
			memcpy(tocluster->aglcpu, getagldata->aglcpuload, sizeof(tocluster->aglcpu));
			tocluster->aglmemoryavail = getagldata->aglmemoryload[1];
			memcpy(tocluster->toaglapp, getagldata->aglappdata, sizeof(tocluster->toaglapp));
			tocluster->ivifps = getagldata->ivifps;
			tocluster->alexafps = getagldata->alexafps;
		}
	}
	
	//if(getmonitflag == 1){
	//	memcpy(&tomonit, tocluster, sizeof(struct Todataapp));
	//	getmonitflag = 0;
	//}
	//usleep(200*1000);
	
	//int i;
	//printf("\n>>>\n");
	//printf("CPU load:\n");
	//printf("	CPU states:	 %.1f%\n",tocluster->qnxcpu[0]);
	//for(i=1; i<7; i++){
	//	printf("	  CPU[%d] states:	 %.1f%\n",i,tocluster->qnxcpu[i]);
	//}
	//printf("\n");
	//printf("	AGL vCPU states:	 %.1f%\n",tocluster->aglcpu[0]);
	//for(i=1; i<5; i++){
	//	printf("	  AGL vCPU[%d] states:	 %.1f%\n",i,tocluster->aglcpu[i]);
	//}	
    //
    //printf("\n");
	//printf("Memory load:\n");
	//printf("	Board total: %dM\n",tocluster->totalmemory);
	//printf("	QNX total  : %dM\n",tocluster->qnxmemory);
	//printf("	QNX avail  : %dM\n\n",tocluster->qnxmemoryavail);
	//printf("	AGL total  : %dM\n",tocluster->aglmemory);
	//printf("	AGL avail  : %dM\n",tocluster->aglmemoryavail);
	//
	//printf("\n");
	//printf("GPU load:\n");
	//printf("	Frames per second       : %.1fFPS\n",tocluster->gpufps);
	//printf("	Tiler active            : %.1f%\n",tocluster->gputiler);
	//printf("	Shader processing load  : %.1f%\n",tocluster->gpushader);
	//printf("	Renderer active         : %.1f%\n",tocluster->gpurenderer);
	//
	//printf("\n");
	//printf("QNX process:\n");
	//for(i=0; i<6; i++)
	//	printf("%d       %.1f       %.1fMB       %s\n", tocluster->toqnxapp[i].pid, tocluster->toqnxapp[i].cpuload, tocluster->toqnxapp[i].memoryload, tocluster->toqnxapp[i].name);
	//
	//printf("\n");
	//printf("AGL process:\n");    
	//for(i=0; i<6; i++)            
	//	printf("%d       %.1f       %.1fMB       %s\n", tocluster->toaglapp[i].pid, tocluster->toaglapp[i].cpuload, tocluster->toaglapp[i].memoryload, tocluster->toaglapp[i].name);
    //
	//printf("Bus Monitoring:\n");
	//for(i=0; i<10; i++)            
	//	printf("	%d %s		%d %s   	%s \n", tocluster->busmonidata[i].rdata, tocluster->busmonidata[i].rdatacom,tocluster->busmonidata[i].wdata, tocluster->busmonidata[i].wdatacom,tocluster->busmonidata[i].name);

}
