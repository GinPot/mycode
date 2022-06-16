#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


int MemoryLoadFu(float *memoryload)
{
    static FILE *memfd = NULL;
    char buff[30];
	
	if (!memfd) {
		if (!(memfd = fopen("/proc/meminfo","r"))){
			fprintf(stderr, "Failed /proc/meminfo open: %s", strerror(errno));
			return -1;
		}
	}

	rewind(memfd);
	fflush(memfd);
    if (!fgets(buff, sizeof(buff), memfd)){
		printf("Failed fgets /proc/meminfo!\n");
		return -1;
	}
	memoryload[0] = (float)atoi(buff+9)/1024;			//total memory
	
    if (!fgets(buff, sizeof(buff), memfd)){
		printf("Failed fgets /proc/meminfo!\n");
		return -1;
	}
	
    if (!fgets(buff, sizeof(buff), memfd)){
		printf("Failed fgets /proc/meminfo!\n");
		return -1;
	}
	memoryload[1] = (float)atoi(buff+13)/1024;			//Available memory
	
	//printf("MemTotal: %dM\n",memoryload[0]);
	//printf("MemAvailable: %dM\n",memoryload[1]);

	return 0;
}
