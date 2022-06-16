#include <stdio.h>
#include <stdlib.h>

#include "memoryload.h"



int GetMemoryLoad(int *memoryload)
{
	int i;
	FILE *fp = NULL;
	char buf[30];
	
	if (!(fp = fopen("/proc/vm/stats", "r")))
	{
		printf("Failed fopen /proc/vm/stats!\n");
		return -1;	
	}
	
	for(i=0; i<8; i++)
		if (!fgets(buf, sizeof(buf), fp))
		{	
			printf("Failed fgets /proc/vm/stats!\n");
			goto fail;		
		}
		
	if (!fgets(buf, sizeof(buf), fp)) 
	{
		printf("Failed fgets /proc/vm/stats!\n");	
		goto fail;		
	}	

	*memoryload = atoi(buf+20);

	//printf("memory load : %d\n",memoryload[2]);
	fclose(fp);
    return 0;
fail:
	fclose(fp);
	return -1;
}
