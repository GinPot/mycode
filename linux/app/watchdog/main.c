#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdlib.h>

#include <linux/watchdog.h>
int main()
{

	int wd_fd;	 
	wd_fd = open("/dev/watchdog",O_RDWR);
	if(wd_fd < 0)  
	{  
		printf("wd open failed %d\n",wd_fd);  
		return;
	}
	int timeout = 20;
 
    ioctl(wd_fd, WDIOC_SETOPTIONS, WDIOS_ENABLECARD);
    ioctl(wd_fd, WDIOC_SETTIMEOUT, &timeout);
	
	while(1)
	{
		sleep(10);
		printf("watchdog...\n");  
		ioctl(wd_fd,WDIOC_KEEPALIVE,NULL);
	}

      return 0;
}