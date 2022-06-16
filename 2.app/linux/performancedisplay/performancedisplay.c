#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <pthread.h>

#include "cpuload.h"
#include "memoryload.h"
#include "topprocess.h"
#include "getfps.h"



struct AglBuffda {
	int ivifps;
	int alexafps;
	float aglcpuload[5];
	float aglmemoryload[2];
	struct TopProce aglappdata[PROCESSNUM];
};
struct AglBuffda *agldatabu;


void getfps(void)
{
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
	int i,j,fd,len,ret;
	char * agldatatmp;
	pthread_t tidp;

	agldatabu = malloc(sizeof(struct AglBuffda));
	memset(agldatabu, 0, sizeof(struct AglBuffda));
	agldatatmp = (char *)agldatabu;
	
    /* creat pthread */
    if ((pthread_create(&tidp, NULL, GetFpsPthread, NULL)) == -1)
    {
        printf("create pthread error!\n");
        return  0; 
    }
	
	fd = open("/dev/dev_perform", O_RDWR);
	if (fd < 0){
		printf("can't open /dev/dev_perform\n");
		free(agldatabu);
		return -1;
	}

	while(1)
	{
		GetCpuLoad(agldatabu->aglcpuload);
		MemoryLoadFu(agldatabu->aglmemoryload);
		TopCpuPro(agldatabu->aglappdata);

		//printf("\n》》》\n");
		//printf("VCPU load：\n");
		//printf("	vCPU states: %.1f%\n",(100-agldatabu->aglcpuload[0]));
		//for(i=1; i<5; i++)
		//	printf("	vCPU[%d] states: %.1f%\n",i,(100-agldatabu->aglcpuload[i]));
        //
		//printf("Memory load：\n");
		//printf("	AGL total  :%.0fM\n",agldatabu->aglmemoryload[0]);
		//printf("	AGL avail  :%.0fM\n",agldatabu->aglmemoryload[1]);
        //
		//printf("AGL process：\n");
		//for(i=0; i<PROCESSNUM; i++){
		//	printf(" %.1f %.1fMB %d %s \n",agldatabu->aglappdata[i].cpuload,agldatabu->aglappdata[i].memoryload,agldatabu->aglappdata[i].pid,agldatabu->aglappdata[i].name);
		//}
		
		//printf("agldatabu->ivifps = %d\n", agldatabu->ivifps);
		//printf("agldatabu->alexafps = %d\n", agldatabu->alexafps);
		
		i = write(fd, agldatatmp, sizeof(struct AglBuffda));

		usleep(1000000);
		//getfps();
	}

	free(agldatabu);
	close(fd);
    return 0;
}
