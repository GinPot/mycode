#include <sys/iomgr.h>
#include <sys/siginfo.h>
#include <hyp_shm.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <screen/screen.h>
#include <dlfcn.h>




int main(int argc, char *argv[]) 
{
	int i;
	FILE *fp = NULL;
	char buf[30];
	
	if (!(fp = fopen("/proc/vm/stats", "r")))
		printf("Failed fopen /proc/vm/stats!\n");
	
	for(i=0; i<8; i++)
		if (!fgets(buf, sizeof(buf), fp))
			printf("Failed fgets /proc/vm/stats!\n");
	
		
	if (!fgets(buf, sizeof(buf), fp)) 
		printf("Failed fgets /proc/vm/stats!\n");	
		
	printf("memory load : %s\n",buf);
		
		
    return 0;
}
