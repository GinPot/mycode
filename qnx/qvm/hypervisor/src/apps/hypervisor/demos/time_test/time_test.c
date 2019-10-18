#include <hyp_shm.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/dispatch.h>
#include <dlfcn.h>



int main(int argc, char *argv[])
{
	struct  timezone   tz;
	static float oldtime;
	static struct timeval time1;

	gettimeofday(&time1, &tz);
	oldtime = time1.tv_sec + (float)time1.tv_usec/1000000;
	
	printf("time:%f~~~~~~~~~~~~~~~\n",oldtime);
	
    return 0;
}
