#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#include "fifo.h"

#define CPUCORENUM	4
#define CPUTIMES	(CPUCORENUM+1)

Queue idata[CPUTIMES],totaldata[CPUTIMES];

static int util_get_cpu_usage(float *cpu_usage)
{
	static FILE *fp = NULL;
	char buf[256];
	char cpucore[4];
	uint64_t tot;
	uint64_t u, n, s, i, w, x, y, z;
	//static uint64_t last_i[CPUTIMES] = {0}, last_total[CPUTIMES] = {0};
	static uint64_t cur_i[CPUTIMES]={0}, cur_total[CPUTIMES]={0}, total_i[CPUTIMES]={0}, total_total[CPUTIMES]={0};
	static uint64_t total_iold[CPUTIMES]={0}, total_totalold[CPUTIMES]={0};
	uint64_t popilast,poptotallast;
	int j;

	if (!fp) {
		if (!(fp = fopen("/proc/stat", "r"))){
			fprintf(stderr, "Failed /proc/stat open: %s", strerror(errno));
			return -1;
		}
	}
	
	rewind(fp);
	fflush(fp);
	for(j=0; j<CPUTIMES; j++)
	{
		if (!fgets(buf, sizeof(buf), fp)) {
			fprintf(stderr, "failed /proc/stat read\n");
			return -1;
		} else {
			sscanf(buf, "%s %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
					cpucore,
					&u,
					&n,
					&s,
					&i,
					&w,
					&x,
					&y,
					&z
				  );
			cur_i[j] = i;
			cur_total[j] = u+n+s+i+w+x+y+z;
			
				  
			if(idata[j].m_size == capacity){
				popilast = Pop(&idata[j]);
				poptotallast = Pop(&totaldata[j]);
				
				total_i[j] = total_i[j] - popilast + cur_i[j];
				total_total[j] = total_total[j] - poptotallast + cur_total[j];
				
				Push(&idata[j], cur_i[j]);
				Push(&totaldata[j], cur_total[j]);
		
				//printf("first=%d   mo=%d  procputimetotal=%d   m_size=%d\n",totalcputimecur,totalcputimelast,totalputimetotal,procpuload.m_size);
			}else{
				Push(&idata[j], cur_i[j]);
				Push(&totaldata[j], cur_total[j]);
				if(IsFull(&idata[j]) == true){
					for(i=0;i<capacity;i++){
						total_i[j] += idata[j].m_array[i];
						total_total[j] += totaldata[j].m_array[i];
					}
				}
			}  

			if(0 != total_total[j]-total_totalold[j]){ 
				cpu_usage[j]=(1.0 - ((float)(total_i[j]-total_iold[j])/(float)(total_total[j]-total_totalold[j])))*100;
			}
			total_iold[j] = total_i[j];
			total_totalold[j] = total_total[j];
			
/*			if (last_total[j] == 0) {
				last_total[j] = u+n+s+i+w+x+y+z;
				last_i[j] = i;
				usleep(30000);
			} else {
				tot = u+n+s+i+w+x+y+z;
				cpu_usage[j] = (1.0 - ((float)(i-last_i[j])/(float)(tot-last_total[j])))*100;
				last_i[j] = i;
				last_total[j] = tot;
			}
*/			
		}
	}
	
	return 0;
}

void GetCpuInit(void)
{
	int j;

	for(j=0; j<CPUTIMES; j++){
		InitQueue(&idata[j]);
		InitQueue(&totaldata[j]);
	}

}

/*****************************************************************
*Function	: GetCpuload
*Description: 获取CPU各核使用率
*parameter	: cpu_load[0]为总CPU使用率，后面为各核使用率
*Return		: 0为成功; -1为失败
*Others		:
*****************************************************************/
int GetCpuLoad(float *cpu_load)
{
	int ret,j;
	
	ret = util_get_cpu_usage(cpu_load);
	if(ret != 0){
		return -1;
	}	
	//printf("\n");
	//for(j=0; j<5; j++){
	//	printf("	cpu[%d] = %.1f\n", j,((1-cpu_load[j])*100));
	//}

	return 0;
}
