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
	char buf_vir[5];
	
	//if ((argc != 3) && (argc != 4))
	//{
	//	print_usage(argv[0]);
	//	return -1;
	//}

	fd = open("/dev/virt_touch", O_RDWR);
	if (fd < 0)
	{
		printf("can't open /dev/virt_touch\n");
		return -1;
	}
	
	while(1)
	{
		scanf("%d%d%d",&buf[0],&buf[1],&buf[2]);
		
		
		printf("%d   %d   %d\n",buf[0],buf[1],buf[2]);
		
		buf_vir[0] = buf[0] >> 8;
		buf_vir[1] = buf[0];
		
		buf_vir[2] = buf[1] >> 8;
		buf_vir[3] = buf[1];
		
		buf_vir[4] = buf[2];
		
		printf("%d   %d   %d   %d   %d\n",buf_vir[0],buf_vir[1],buf_vir[2],buf_vir[3],buf_vir[4]);
		
		rt = write(fd, buf_vir, 5);
	}
	
	
	
	

	//if (strcmp(argv[1], "r") == 0)
	//{
	//	buf[0] = strtoul(argv[2], NULL, 0);
	//	read(fd, buf, 1);
	//	printf("data: %c, %d, 0x%2x\n", buf[0], buf[0], buf[0]);
	//}
	//else if (strcmp(argv[1], "w") == 0)
	//{
	//	buf[0] = strtoul(argv[2], NULL, 0);
	//	buf[1] = strtoul(argv[3], NULL, 0);
	//	write(fd, buf, 2);
	//}
	//else
	//{
	//	print_usage(argv[0]);
	//	return -1;
	//}
	
	return 0;
}
