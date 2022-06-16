#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>







int main(int argc, char *argv[])
{
	int buff[10];

	printf("flash cache test.\n");

	__dma_inv_area(buff, 10);








	return 0;
}







