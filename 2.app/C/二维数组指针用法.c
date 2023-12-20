#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

//int (*Max96752_registers)[3];										//二维数组的指针，
//int *Max96752_registers[3]; = int *(Max96752_registers[3]);		//保存指针的一维数组

static int Max96752_registers_B[][3] =
{
	{0x00, 0x02, 0x43},
	{0x01, 0x40, 0x20},
	{0x02, 0x1E, 0x84},
};

int main(int argc, char *argv[])
{
	int max96752_registers_num;
	int (*Max96752_registers)[3];
	
	max96752_registers_num = sizeof(Max96752_registers_B)/sizeof(Max96752_registers_B[0]);
	
	Max96752_registers = Max96752_registers_B;
	
	
	printf("max96752_registers_num=%d\n", max96752_registers_num);

	printf("0x%x,0x%x,0x%x\n", Max96752_registers[0][0], Max96752_registers[0][1], Max96752_registers[0][2]);
	printf("0x%x,0x%x,0x%x\n", Max96752_registers[1][0], Max96752_registers[1][1], Max96752_registers[1][2]);
	printf("0x%x,0x%x,0x%x\n", Max96752_registers[2][0], Max96752_registers[2][1], Max96752_registers[2][2]);
	
	return 0;
}