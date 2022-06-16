#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>


void getfps(void)
{
	static int fs_numb=0;
	struct timeval endtime;
	static double timeuse, old_timeuse;
	
	fs_numb++;
	gettimeofday(&endtime,0);
	timeuse = 1000000*(endtime.tv_sec) + endtime.tv_usec;
	timeuse = timeuse/1000000;	
	if(timeuse > old_timeuse+1)
	{
		old_timeuse = timeuse;
		
		printf("timeuse = %f   fs_numb = %d fs\n",timeuse,fs_numb);
		
		fs_numb = 0;
	}	
}

//#include <sys/time.h>
//#include <sched.h>
	//pid_t pid = getpid();
	//struct sched_param param;
	//param.sched_priority = sched_get_priority_max(SCHED_FIFO);   // 也可用SCHED_RR
	//sched_setscheduler(pid, SCHED_RR, &param);                   // 设置当前进程
	//pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);   // 设置当前线程
	//nice(-20); 