#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>







int main(int argc, char *argv[])
{
	int gpio_fd,rt;
	char buf;
	
	
	gpio_fd = open("/sys/devices/soc/24.tsc/drv_irq", O_RDWR);
	while(1){
		scanf("%s",&buf);
		
		rt = write(gpio_fd, &buf, 1);
		if(rt < 0)
			printf("write fail\n");
	}
	return 0;
}