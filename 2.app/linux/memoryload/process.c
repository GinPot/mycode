#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>

#include "fifo.h"
 
#define VMRSS_LINE 22

#define PROCESS_ITEM 14
 
typedef struct {
	unsigned long user;
	unsigned long nice;
	unsigned long system;
	unsigned long idle;
	unsigned long iowait;
	unsigned long irq;
	unsigned long softirq;
}Total_Cpu_Occupy_t;
 
 
typedef struct {
	unsigned int pid;
	unsigned long utime;  //user time
	unsigned long stime;  //kernel time
	unsigned long cutime; //all user time
        unsigned long cstime; //all dead time
}Proc_Cpu_Occupy_t;
 
Queue procpuload,totalcpuload;
//获取第N项开始的指针
const char* get_items(const char*buffer ,unsigned int item){
	
	const char *p =buffer;
 
	int len = strlen(buffer);
	int count = 0;
	
	for (int i=0; i<len;i++){
		if (' ' == *p){
			count ++;
			if(count == item -1){
				p++;
				break;
			}
		}
		p++;
	}
 
	return p;
}

//获取总的CPU时间
unsigned long get_cpu_total_occupy(){
	
	FILE *fd;
	char buff[1024]={0};
	Total_Cpu_Occupy_t t;
 
	fd =fopen("/proc/stat","r");

		
	fgets(buff,sizeof(buff),fd);
	char name[64]={0};
	sscanf(buff,"%s %ld %ld %ld %ld %ld %ld %ld",name,&t.user,&t.nice,&t.system,&t.idle,&t.iowait,&t.irq,&t.softirq);
	fclose(fd);
	
	return (t.user + t.nice + t.system + t.idle + t.iowait + t.irq + t.softirq);
}

//获取进程的CPU时间
unsigned long get_cpu_proc_occupy(unsigned int pid){
	
	char file_name[64]={0};
	Proc_Cpu_Occupy_t t;
	FILE *fd;
	char line_buff[1024]={0};
	sprintf(file_name,"/proc/%d/stat",pid);
	
	fd = fopen(file_name,"r");

	
	fgets(line_buff,sizeof(line_buff),fd);
	
	sscanf(line_buff,"%u",&t.pid);
	const char *q =get_items(line_buff,PROCESS_ITEM);
	sscanf(q,"%ld %ld %ld %ld",&t.utime,&t.stime,&t.cutime,&t.cstime);
	fclose(fd);
	
	return (t.utime + t.stime + t.cutime + t.cstime);
}

//获取CPU占用率
float get_proc_cpu(unsigned int pid){
	int i;
 	float pcpu = 0.0;
	unsigned long totalcputimecur,procputimecur,totalcputimelast,procputimelast;
	static unsigned long procputimeold,totalcputimeold,procputimetotal,totalputimetotal;
	
	procputimecur=get_cpu_proc_occupy(pid);
	totalcputimecur=get_cpu_total_occupy();

	if(procpuload.m_size == capacity){
		procputimelast = Pop(&procpuload);
		totalcputimelast = Pop(&totalcpuload);
		
		procputimetotal = procputimetotal - procputimelast + procputimecur;
		totalputimetotal = totalputimetotal - totalcputimelast + totalcputimecur;
		
		Push(&procpuload, procputimecur);
		Push(&totalcpuload, totalcputimecur);

		//printf("first=%d   mo=%d  procputimetotal=%d   m_size=%d\n",totalcputimecur,totalcputimelast,totalputimetotal,procpuload.m_size);
	}else{
		Push(&procpuload, procputimecur);
		Push(&totalcpuload, totalcputimecur);
		if(IsFull(&procpuload) == true){
			for(i=0;i<capacity;i++){
				procputimetotal += procpuload.m_array[i];
				totalputimetotal += totalcpuload.m_array[i];
			}
		}
	}

	
	if(0 != totalputimetotal-totalcputimeold){ 
		pcpu=100.0 * (procputimetotal-procputimeold)/(totalputimetotal-totalcputimeold);
	}
	procputimeold = procputimetotal;
	totalcputimeold = totalputimetotal;
	
	return pcpu;
}

//获取进程占用内存
unsigned int get_proc_mem(unsigned int pid){
	
	char file_name[64]={0};
	FILE *fd;
	char line_buff[512]={0};
	sprintf(file_name,"/proc/%d/status",pid);
	
	fd =fopen(file_name,"r");

	
	char name[64];
	int vmrss;
	for (int i=0; i<VMRSS_LINE-1;i++){
		fgets(line_buff,sizeof(line_buff),fd);
	}
	
	fgets(line_buff,sizeof(line_buff),fd);
	sscanf(line_buff,"%s %d",name,&vmrss);
	fclose(fd);
 
	return vmrss;
}

int main(int argc, char *argv[])
{
	unsigned int pid=0;

	
	InitQueue(&procpuload);
	InitQueue(&totalcpuload);
	pid = atoi(argv[1]);
	
	while(1)
	{
		//printf("pid=%d\n",pid);
		printf("pcpu=%.1f\n",get_proc_cpu(pid)*4);
		//printf("procmem=%.1fM\n",(float)get_proc_mem(pid)/1024);
		
		
		usleep(200*1000);
	}

	return 0;
}
