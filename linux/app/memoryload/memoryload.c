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

int main(int argc, char *argv[])
{
	int memoryload[2];
	
    FILE * fd;
    char buff[30];
    fd = fopen("/proc/meminfo","r");
	
    if (!fgets(buff, sizeof(buff), fd))
		printf("Failed fgets /proc/meminfo!\n");
	memoryload[0] = atoi(buff+9)/1024;
	
    if (!fgets(buff, sizeof(buff), fd))
		printf("Failed fgets /proc/meminfo!\n");
	
    if (!fgets(buff, sizeof(buff), fd))
		printf("Failed fgets /proc/meminfo!\n");
	memoryload[1] = atoi(buff+13)/1024;
	
	printf("MemTotal: %dM\n",memoryload[0]);
	printf("MemAvailable: %dM\n",memoryload[1]);
	return 0;
}
