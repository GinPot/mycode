#include <stdio.h>
#include <string.h>





// gcc main.c
int main(int argc, char *argv[])
{
/**************************sizeof和strlen差异**********************************/
//	char chr[] = "Common C language API testing";
//	printf("%s, sizeof=%ld  strlen=%ld\n", chr, sizeof(chr), strlen(chr));

//解释
//	//sizeof计算传入参数所占内存空间大小,在传入参数是字符串时会包含“\0”;
//	//strlen计算字符串的大小,不包含“\0”;
//	//printf: Common C language API testing, sizeof=30  strlen=29

/**************************字符串存储差异**********************************/
//	char aa[]= "aaaaaaaaaaa";			//定义了一个数组,编译的时候会将字符串保存在rodata段中，代码运行的时候将radata对应的内容复制到aa对应的栈地址上;
//	char *cc = "cccccccccc";			//定义了一个指针,编译的时候会将字符串保存在rodata段中，c指向rodata的一个只读地址(程序加载的时候自动申请分配的虚拟地址),所以无法修改
//	printf("aa=%p cc=%p\n", aa, cc);			//地址发现不是连续，但反汇编是连续的，也侧面证明cc是加载a.out申请的一段只读地址保存，aa是运行时从只读位置复制到栈中

//解释
//	objdump -s -d a.out > main.txt 反汇编可以得到存储段的信息
	
	

	return 0;
}