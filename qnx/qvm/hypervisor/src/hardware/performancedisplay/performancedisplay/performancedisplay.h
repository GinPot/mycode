/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */

#ifndef PERFORMANCEDISPLAY_H_
#define PERFORMANCEDISPLAY_H_

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

void initperform(void);
void getperform(struct Todataapp *tocluster);


#endif

