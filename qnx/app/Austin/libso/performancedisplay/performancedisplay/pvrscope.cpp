#include <stdio.h>
#include <unistd.h>




#include "PVRScopeStats.h"
extern "C" {
//Internal control data 
static struct SPVRScopeImplData *psData;  
//Counter information (set at uint time) 
static struct SPVRScopeCounterDef *psCounters; 
static struct SPVRScopeCounterReading sReading;
static unsigned int uCounterNum;  

// Step 1. Define a function to initialise PVRScopeStats 
unsigned int PSInit(struct SPVRScopeImplData **ppsPVRScopeData, struct SPVRScopeCounterDef **ppsCounters, struct SPVRScopeCounterReading* const psReading, unsigned int* const pnCount) 
{ 
	//Initialise PVRScope 
	const enum EPVRScopeInitCode eInitCode = PVRScopeInitialise(ppsPVRScopeData); 
	if(ePVRScopeInitCodeOk == eInitCode) 
	{ 
		//printf("Initialised services connection.\n"); 
	} 
	else 
	{ 
		printf("Error: failed to initialise services connection.\n"); 
		*ppsPVRScopeData = NULL; 
		return -1; 
	} 
	//Initialise the counter data structures. 
	PVRScopeGetCounters(*ppsPVRScopeData, pnCount, ppsCounters, psReading);
	//if (PVRScopeGetCounters(*ppsPVRScopeData, pnCount, ppsCounters, psReading)) 
	//{ 
	//	printf("Total counters enabled: %d.", *pnCount); 
	//} 
	return 0; 
}

int PVRSInit(void)
{
	int ret;

	ret = PSInit(&psData, &psCounters, &sReading, &uCounterNum);
	if(ret != 0)
		return -1; 
	
	return 0; 
}

int PVRload(float *gpuload)
{
	unsigned int i = 0,j=1;
	
	PVRScopeSetGroup(psData, 0); 
	while (j) 
	{ 
		for(i = 0; i < 20; ++i)
		{
			PVRScopeReadCounters(psData, NULL);
			usleep(50*1000);
		}
        if(PVRScopeReadCounters(psData, &sReading))
        {
			//printf("PVRScopeReadCounters %d\n",sReading.nValueCnt);
            //for(int i = 0; i < uCounterNum; ++i)
            //    if(i < sReading.nValueCnt)
            //        printf("Group %d %s : %f\%\n", sReading.nReadingActiveGroup,psCounters[i].pszName, sReading.pfValueBuf[i]);
            
			if(48 == sReading.nValueCnt)
			{
				//printf("Group %d %s : %f\%\n", sReading.nReadingActiveGroup,psCounters[2].pszName, sReading.pfValueBuf[2]);
				//printf("Group %d %s : %f\%\n", sReading.nReadingActiveGroup,psCounters[37].pszName, sReading.pfValueBuf[37]);
				//printf("Group %d %s : %f\%\n", sReading.nReadingActiveGroup,psCounters[9].pszName, sReading.pfValueBuf[9]);
				//printf("Group %d %s : %f\%\n", sReading.nReadingActiveGroup,psCounters[39].pszName, sReading.pfValueBuf[39]);
				gpuload[0] = sReading.pfValueBuf[2];
				gpuload[1] = sReading.pfValueBuf[37];
				if(gpuload[1] > 100){
					gpuload[1] = 0;
				}
				gpuload[2] = sReading.pfValueBuf[9];
				if(gpuload[2] > 100){
					gpuload[3] = 0;
				}
				gpuload[3] = sReading.pfValueBuf[39];
				if(gpuload[4] > 100){
					gpuload[4] = 0;
				}
			}
        }
		j--;
		//Poll for the counters once in 50 msecond 
		//sleep(1); 
	} 
	// Step 5. Shutdown PVRScopeStats 
	//PVRScopeDeInitialise(&psData, &psCounters, &sReading);

	return 0;
}

}