#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "PVRScopeStats.h"

// Step 1. Define a function to initialise PVRScopeStats
bool PSInit(SPVRScopeImplData **ppsPVRScopeData, SPVRScopeCounterDef **ppsCounters, SPVRScopeCounterReading* const psReading, unsigned int* const pnCount)
{
//Initialise PVRScope
    const EPVRScopeInitCode eInitCode = PVRScopeInitialise(ppsPVRScopeData);

    if(ePVRScopeInitCodeOk == eInitCode)
    {
        printf("Initialised services connection.\n");
    }
    else
    {
        printf("Error: failed to initialise services connection.\n");
        *ppsPVRScopeData = NULL;
        return false;
    }

	//Initialise the counter data structures.
	if (PVRScopeGetCounters(*ppsPVRScopeData, pnCount, ppsCounters, psReading))
	{
        printf("Total counters enabled: %d.", *pnCount);
    }

    return true;
}

int main()
{
    //Internal control data
    SPVRScopeImplData *psData;

    //Counter information (set at uint time)
    SPVRScopeCounterDef             *psCounters;
    unsigned int                    uCounterNum;

    //Counter reading data
    unsigned int                    uActiveGroupSelect;
    bool                            bActiveGroupChanged;
    SPVRScopeCounterReading sReading;


    // Step 2. Initialise PVRScopeStats
    if (PSInit(&psData, &psCounters, &sReading, &uCounterNum))
            printf("PVRScope up and running.");
    else
            printf("Error initializing PVRScope.");

    //Print each and every counter (and its group)
    printf("Find below the list of counters:");
    for(int i = 0; i < uCounterNum; ++i)
    {
            printf("    Group %d %s", psCounters[i].nGroup, psCounters[i].pszName);
    }

    // Step 3. Set the active group to 0
    bActiveGroupChanged = true;
    uActiveGroupSelect = 0;

    unsigned int sampleRate = 100;
    unsigned int index = 0;

    unsigned int i = 0;
    while (1)
    {
        // Ask for the active group 0 only on the first run.
        if(bActiveGroupChanged)
        {
            PVRScopeSetGroup(psData, uActiveGroupSelect);
            bActiveGroupChanged = false;
        }

        ++index;
        if (index < sampleRate)
        {
			// Sample the counters every 10ms. Don't read or output it.
			PVRScopeReadCounters(psData, NULL);
		}
        else
        {
            ++i;
            index = 0;

			// Step 4. Read and output the counter information for group 0 to Logcat
			if(PVRScopeReadCounters(psData, &sReading))
			{
				 printf("PVRScopeReadCounters %d\n",sReading.nValueCnt);
				// Check for all the counters in the system if the counter has a value on the given active group and ouptut it.
				for(int i = 0; i < uCounterNum; ++i)
				{
					if(i < sReading.nValueCnt)
					{
						printf("Group %d %s : %f\%\n", sReading.nReadingActiveGroup,psCounters[i].pszName, sReading.pfValueBuf[i]);
					}
				}
			}
        }

        //Poll for the counters once a second
        usleep(10 * 1000);
    }

    // Step 5. Shutdown PVRScopeStats
    PVRScopeDeInitialise(&psData, &psCounters, &sReading);
}
