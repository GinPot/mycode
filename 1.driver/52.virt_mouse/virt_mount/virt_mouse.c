#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char **argv)
{
	int fd,rt;
	int buf[3];
	//char buf_vir[5];

	fd = open("/dev/virt_mouses", O_RDWR);
	if (fd < 0)
	{
		printf("can't open /dev/virt_mouses\n");
		return -1;
	}
	
	while(1)
	{
		scanf("%d%d%d",&buf[0],&buf[1],&buf[2]);
		//buf[0]	X相对坐标
		//buf[1]	Y相对坐标
		//buf[2]	左键是否按下

		rt = write(fd, buf, 12);
	}

	
	return 0;
}
