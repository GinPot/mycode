#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>



int main(int argc, char *argv[])
{

	int fd;

    fd = open("/dev/shmem/ser1_tx.log", O_RDONLY | O_TRUNC);	//O_TRUNC │若文件存在，则长度被截为0，属性不变
    if (-1 == fd)
    {
        printf("open error\n");
        return 0;
    }
	printf("Empty file successfully.\n");
	
	
    close(fd);

    return 0;
}


