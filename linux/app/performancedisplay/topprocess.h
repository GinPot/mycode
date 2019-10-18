#ifndef _TOP_PROCESS
#define _TOP_PROCESS


#define PROCESSNUM	6

struct TopProce {
	int pid;
	float cpuload;
	float memoryload;
	char name[15];
};

extern int TopCpuPro(struct TopProce *procedata);


#endif