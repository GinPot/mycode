#include <linux/module.h>
#include <linux/device.h>


#include <linux/timex.h> 
#include <linux/delay.h>
#include <linux/sched/clock.h>

static int __init time_sleep_test_init(void)
{
	struct timespec64 ts_start;
	
	printk( "GinPot: %s, jiffies_64=%llu jiffies=%lu INITIAL_JIFFIES=%ld HZ=%d bootime=%llu\n", __func__, jiffies_64, jiffies, ( long)( int)INITIAL_JIFFIES, HZ, jiffies_64/HZ);
	printk( "GinPot: %s, sizeof(unsigned char)=%lu sizeof(unsigned short)=%lu sizeof(unsigned int)=%lu sizeof(unsigned long)=%lu sizeof(unsigned long long)=%lu\n", __func__, sizeof(unsigned char), sizeof(unsigned short), sizeof(unsigned int), sizeof(unsigned long), sizeof(unsigned long long));

/*
忙等待：
	ndelay(unsigned long nsecs)	//小于10us首选使用
	udelay(unsigned long usecs)
	mdelay(unsigned long msecs)
*/

	ktime_get_real_ts64(&ts_start);
	printk("CLOCK_REALTIME : %lld s, %lu ns\n", ts_start.tv_sec, ts_start.tv_nsec);				//CLOCK_REALTIME： 离linux epoch(1970-01-01 00:00:00)过了多长时间

	ssleep(1);					//大于1s使用,一般会多睡10ms

	ktime_get_ts64(&ts_start);																	//获取上一次的tick是的墙上时间秒+获取当前最新的ns减去最后一次tick记录的ns得到当前的ns，再转换为monoton时间
	printk("CLOCK_MONOTONIC : %lld s, %lu ns\n", ts_start.tv_sec, ts_start.tv_nsec);			//CLOCK_MONOTONIC： 离开机启动过了多长时间

	msleep(10);					//大于10ms使用，一般实际睡眠时间会大于设定10ms左右

	ktime_get_raw_ts64(&ts_start);
	printk("CLOCK_MONOTONIC_RAW : %lld s, %lu ns\n", ts_start.tv_sec, ts_start.tv_nsec);		//CLOCK_MONOTONIC_RAW： 离开机启动过了多长时间

	usleep_range(10, 20000);	//在(10us - 20ms)之前使用，上面2个精度不高，这个基于hrtime精度高；  调度器可以自由地将您的唤醒与可能由于其他原因发生的任何其他唤醒合并，或者在最坏的情况下，为您的上限触发一个中断

	ktime_get_boottime_ts64(&ts_start);
	printk("CLOCK_BOOTTIME : %lld s, %lu ns\n", ts_start.tv_sec, ts_start.tv_nsec);				//CLOCK_BOOTTIME： 离开机启动过了多长时间

	printk("sched_clock :  %llu ns\n", sched_clock());											//基于jiffies(arm timer未初始化之前)，获取当前时间点到系统启动之间的纳秒值
	return 0;
}

static void __exit time_sleep_test_exit(void)
{
	printk( "GinPot: %s.\n", __func__);

}

MODULE_LICENSE("GPL");
module_init(time_sleep_test_init);
module_exit(time_sleep_test_exit);

/*
msleep(timeout)
	schedule_timeout_uninterruptible(timeout)
		schedule_timeout(timeout)
			timer_setup_on_stack(&timer.timer, process_timeout, 0)
			__mod_timer(&timer.timer, expire, 0)
			schedule()



*/


/*******************************************************************************************/

/*
HZ: 固定周期发生的时钟中断数量；
CONFIG_HZ_250=y
CONFIG_HZ=250
表示1秒内发送250次时钟中断，每次4ms

arch\arm64\kernel\vmlinux.lds.S
jiffies = jiffies_64;
jiffies的地址指向jiffies_64，完全等价，都是64bit；
32位机器上，jiffies和jiffies_64的低4个字节是一样的；用get_jiffies_64()获取jiffies_64(分2次读，不是原子操作，增加了lock); 64位机器上get_jiffies_64()和jiffies_64是一样的

unsigned long jiffies/jiffies_64： 全局变量jiffies记录系统自从启动以来的滴答数,初始值设为INITIAL_JIFFIES=((unsigned long)(unsigned int) (-300*HZ))=-75000=4294892296(无符号)
	默认设置-5分钟，在32位系统中开机5分钟发生32位的jiffies 回绕，么做有利于及早暴露设备驱动程序中可能的 jiffies 回绕导致的逻辑错误，方便驱动程序发现错误； 64位的jiffies_64数足够大，基本不会发生溢出,但也沿用了之前的初始值
	HZ=1000，jiffies只需要约49.7天就会产生回绕，jiffies_64可以运行几亿年也不会发生回绕
jiffies/HZ： 可以表示系统启动的时间，单位s秒
*/

/*******************************************************************************************/
/*
sizeof(unsigned char)=1 			%u			2^8   = 255
sizeof(unsigned short)=2 			%u			2^16 = 65535
sizeof(unsigned int)=4 				%u			2^32 = 4294967295
sizeof(unsigned long)=8 			%lu			2^64 = 18446744073709551616
sizeof(unsigned long long)=8		%llu		2^64 = 18446744073709551616

char a[] = "abc";
#define CHAR "abc"
上面字符串定义是都会包换有结束符"\0",

sizeof计算传入参数所占内存空间大小,在传入参数是字符串时会包含“\0”;
strlen计算字符串的大小,不包含“\0”;

*/

/*******************************************************************************************/
/*
void ktime_get_ts64(struct timespec64 *ts)；       //CLOCK_MONOTONIC
void ktime_get_real_ts64(struct timespec64 *)；    //CLOCK_REALTIME
void ktime_get_boottime_ts64(struct timespec64 *)；//CLOCK_BOOTTIME
void ktime_get_raw_ts64(struct timespec64 *)；     //CLOCK_MONOTONIC_RAW

CLOCK_REALTIME： 离linux epoch(1970-01-01 00:00:00)过了多长时间,可以产生断点的方式修改时间，也可以通过NTP修改；
CLOCK_MONOTONIC： 离开机启动过了多长时间，只能通过NTP对时钟进行调整，不会有时间断电，保证了时钟的单调性；
CLOCK_MONOTONIC_RAW： 离开机启动过了多长时间，CLOCK_MONOTONIC的特性，但没有NTP调整；
CLOCK_BOOTTIME： 离开机启动过了多长时间，在系统 suspend 的时候该时钟依然增长；
CLOCK_PROCESS_CPUTIME_ID(2)、CLOCK_THREAD_CPUTIME_ID(3)： 用来计算进程或者线程的执行时间的（用于性能剖析），一旦进程（线程）被切换出去，那么该进程（线程）的clock就会停下



*/

/*******************************************************************************************/

/*

1、原子秒： 铯133原子在能量跃迁时辐射的电磁波的振荡频率;
2、TAI(International Atomic Time)clock:原子秒延展出来的时间轴,精准;
3、GMT： 基于地球自转，公转周期的时间，符合人类习惯，但是又不够精确；
4、UTC(Coordinated Universal Time)：TAI clock的基因（使用原子秒），但是又会适当的调整（leap second），满足人类生产和生活的需要;精准的根据原子频率定义的那个秒来推动钟表的秒针的转动,有一个调节器，在适当的时间，可以把秒针向前或者向后调整一秒来符合人类习惯；
	TAI clock和UTC clock在1972年进行了对准（相差10秒），此后就各自独立运行了。在大部分的时间里，UTC clock跟随TAI clock，除了在适当的时间点，realtime clock会进行leap second的补偿。
	从1972年到2017年，已经有了27次leap second，因此TAI clock的读数已经比realtime clock（UTC时间）快了37秒；
	
5、NTP(Network Time Protocol): 时间同步（不会有间断点，NTP调整的是local oscillator和上游服务器频率误差而已）
6、Linux epoch: 定义为1970-01-01 00:00:00 +0000 (UTC); C语言重写的Unix在1971年发布，为了表明每个文件创建的时间，又为了时间统计，用Linux epoch作为文件创建的时间，在32位系统上只够用68年，在2038年01月19日03时14分07秒会发生翻转，但64位的机器上够用292,277,026,596年12月4日15时30分08秒...


应用层时间获取： 最近的那个Tick对应的时间采样点值再加上一个当前时间点到上一个tick的delta值就精准的定位了当前时间；
	1、time和stime： 秒级别的时间函数，依赖内核中timekeeper模块；					#include <time.h>
	2、gettimeofday和settimeofday： 微秒级别的时间函数，依赖内核中timekeeper模块；	#include <sys/time.h>
	3、clock_gettime和clock_settime： 纳秒级别的时间函数，可以指定系统时钟模块；		#include <time.h>

*/


