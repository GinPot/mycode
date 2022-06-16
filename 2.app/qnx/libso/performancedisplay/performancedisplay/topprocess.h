#ifndef _TOPPROCESS_H_
#define _TOPPROCESS_H_


struct ToQnxProce {
	int pid;
	float cpuload;
	float memoryload;
	char name[15];
};

extern int GetTopPro(struct ToQnxProce *qnxtopdata);



#endif