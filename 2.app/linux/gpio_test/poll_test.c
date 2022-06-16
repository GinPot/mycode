#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <pthread.h>

fd_set exceptfds; 

int main()
{
    int fd=open("/sys/devices/platform/soc/3000000.pinctrl/gpiochip0/gpio/gpio29/value",O_RDONLY);
    if(fd<0)
    {
        perror("open '/sys/devices/platform/soc/3000000.pinctrl/gpiochip0/gpio/gpio29/value' failed!\n");  
        return -1;
    }
    struct pollfd fds[1];
    fds[0].fd=fd;
    fds[0].events=POLLPRI;
    while(1)
    {
		fds[0].events=POLLPRI;
        if(poll(fds,1,-1)==-1)
        {
            perror("poll failed!\n");
            return -1;
        }
        if(fds[0].revents&POLLPRI)
        {
            if(lseek(fd,0,SEEK_SET)==-1)
            {
                perror("lseek failed!\n");
                return -1;
            }
            char buffer[16];
            int len;
            if((len=read(fd,buffer,1))==-1)
            {
                perror("read failed!\n");
                return -1;
            }
            buffer[len]=0;
            printf("%s",buffer);
			printf("11\n");
        }
		printf("22\n");
    }
	
	//char buf[2],ret;
	//while(1){
	//	FD_ZERO(&exceptfds);
	//	FD_SET(fd, &exceptfds);
	//	
	//	ret = select(fd+1,NULL,NULL,&exceptfds,NULL);
	//	if(ret < 0)
	//		printf("select value");
    //
	//	if(ret > 0)
	//	{
	//		/* get value */
	//		ret = lseek(fd, 0, SEEK_SET);
	//		if(ret < 0)
	//			printf("lseek value");
	//		ret = read(fd, buf, 2);
	//		buf[1] = '\0';
	//		printf("read ret = %d, value = %x\n", ret, buf[0]);
	//		
	//		if(ret != 2)
	//			printf("read value");	
	//	}
	//	printf("~~\n");
	//}
    return 0;
}