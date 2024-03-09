#include <stdio.h>
#include <string.h>


//结构体位域
struct Foo2 {					//sizeof(unsigned short)=2
    unsigned short a: 1;		//a占用一个bit，总共占用了16bit，2个字节
    unsigned short b: 3;
    unsigned short : 4;			// 定义匿名位域，选择性跳过部分位不使用
    unsigned short d: 4;
    unsigned short e: 4;
};

int main(int argc, char *argv[])
{
    union {						//联合体、共用体；所有成员占用同一段内存，修改一个成员会影响其余所有成员；占用的内存等于最长的成员占用的内存
        struct Foo2 foo;
        unsigned short s_data;
    }val;

    val.foo.a = 1;
    val.foo.b = 3;
    //val.foo.c = 5;
    val.foo.d = 7;
    val.foo.e = 15;

    printf("val is 0x%x, val_size=%ld Foo2_size=%ld short_size=%ld\n", val.s_data, sizeof(val), sizeof(struct Foo2), sizeof(unsigned short));

    return 0;
}
