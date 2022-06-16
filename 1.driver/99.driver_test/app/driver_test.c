#include <poll.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>


#define IOC_MAGIC 'c'					/*定义设备类型*/
#define IOCINIT _IO(IOC_MAGIC, 0)		/* 初始化设备*/
#define IOCWREG _IOW(IOC_MAGIC, 1, int)	/* 写寄存器 */
#define IOCRREG _IOR(IOC_MAGIC, 2, int)	/* 读寄存器 */


static void test_signal(int signum)
{
	printf("fasync:%d.\n", signum);
}

int main(int argc, char **argv)
{
	int fd, size, i, ret;
	char readbuf[100] = {0};
	char writebuf[] = "write data.";
	char *mmapbuf;
	struct pollfd poll_fds;		//定义一个pollfd结构体key_fds
	int oflags;
	
	fd = open("/dev/driverTest", O_RDWR);
	if(fd < 0){
		printf("open /dev/driverTest fail\n");
	}


	size = write(fd, writebuf, sizeof(writebuf));
	printf("size:%d write: %s\n", size, writebuf);
	
	
	size = read(fd, readbuf, sizeof(readbuf));
	printf("size:%d read: %s\n", size, readbuf);
	

	if(ioctl(fd, IOCINIT, NULL) < 0){
		printf("requre IOCINIT fail\n");
	}	
	if(ioctl(fd, IOCWREG, writebuf) < 0){
		printf("requre IOCINIT fail\n");
	}
	if(ioctl(fd, IOCRREG, readbuf) < 0){
		printf("requre IOCINIT fail\n");
	}	
	printf("read: %s\n", readbuf);
	
	
//	mmapbuf = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//	printf("%p  mmap: \n", mmapbuf);
//	for(i=0; i<10; i++)
//		printf("0x%2x ", mmapbuf[i]);
//	printf("\n");
//	
//	poll_fds.fd = fd;
//	poll_fds.events = POLLIN;
//	
//	while(1){
//		ret = poll(&poll_fds, 1, 5000);
//		if(!ret)
//			printf("time out\n");
//		else{
//			if(poll_fds.revents == POLLIN)
//				printf("poll wake up.\n");
//		}
//	}
	
    signal(SIGIO, test_signal);				//注册一个信号，函数为test_signal
    fcntl(fd, F_SETOWN, getpid());  		//告诉内核，发给谁
    oflags = fcntl(fd, F_GETFL); 			//取得当前的状态
    fcntl(fd, F_SETFL, oflags | FASYNC);  	//改变fasync标记，最终会调用到驱动的faync > fasync_helper：初始化/释放fasync_struct

	while(1){
		sleep(10);
	}
	
	return 0;
}