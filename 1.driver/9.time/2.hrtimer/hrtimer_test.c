#include <linux/module.h>
#include <linux/device.h>

/* 硬、软中断定时器误差都在个位数us(个位数跳动的微小误差) */

static struct hrtimer hrtimer_test;
static enum hrtimer_restart hrtimer_tes_func(struct hrtimer *hrtimer)
{
	printk( "GinPot: %s.\n", __func__);

	
//	return HRTIMER_NORESTART;
/*
HRTIMER_NORESTART,	Timer is not restarted
HRTIMER_RESTART,	Timer must be restarted 
*/
	hrtimer_forward_now(hrtimer, ns_to_ktime(1000000000));
	return HRTIMER_RESTART;
}

static int __init hrtimer_test_init(void)
{
	printk( "GinPot: %s.\n", __func__);

	hrtimer_init(&hrtimer_test, CLOCK_MONOTONIC, HRTIMER_MODE_SOFT);
	hrtimer_test.function = hrtimer_tes_func;
	hrtimer_start(&hrtimer_test, ns_to_ktime(1000000000), HRTIMER_MODE_REL_PINNED_SOFT);

	return 0;
}

static void __exit hrtimer_test_exit(void)
{
	printk( "GinPot: %s.\n", __func__);

	hrtimer_cancel(&hrtimer_test);										//cancel a timer and wait for the handler to finish.
}

MODULE_LICENSE("GPL");
module_init(hrtimer_test_init);
module_exit(hrtimer_test_exit);

/*
hrtimer_init参数说明：

clock_id:
	CLOCK_REALTIME： 离linux epoch(1970-01-01 00:00:00)过了多长时间,可以产生断点的方式修改时间，也可以通过NTP修改；
	CLOCK_MONOTONIC： 离开机启动过了多长时间，只能通过NTP对时钟进行调整，不会有时间断电，保证了时钟的单调性；
	CLOCK_MONOTONIC_RAW： 离开机启动过了多长时间，CLOCK_MONOTONIC的特性，但没有NTP调整；
	CLOCK_BOOTTIME： 离开机启动过了多长时间，在系统 suspend 的时候该时钟依然增长；
	CLOCK_PROCESS_CPUTIME_ID(2)、CLOCK_THREAD_CPUTIME_ID(3)： 用来计算进程或者线程的执行时间的（用于性能剖析），一旦进程（线程）被切换出去，那么该进程（线程）的clock就会停下

mode:
	HRTIMER_MODE_ABS		- Time value is absolute
	HRTIMER_MODE_REL		- Time value is relative to now
	HRTIMER_MODE_PINNED		- Timer is bound to CPU (is only considered when starting the timer)
	HRTIMER_MODE_SOFT		- Timer callback function will be executed in soft irq context,没有设置这个参数即在硬中断处理

*/

/*
硬中断：
[   63.816715] GinPot: hrtimer_tes_func.
[   63.816737] CPU: 1 PID: 0 Comm: swapper/1 Tainted: G           O      5.2.0 #20
[   63.816741] Hardware name: GinPot H5 (DT)
[   63.816746] Call trace:
[   63.816765]  dump_backtrace+0x0/0x168
[   63.816774]  show_stack+0x14/0x20
[   63.816785]  dump_stack+0xa8/0xcc
[   63.816796]  hrtimer_tes_func+0x28/0x58 [hrtimer_test]
[   63.816803]  __hrtimer_run_queues+0x120/0x1b8
[   63.816809]  hrtimer_interrupt+0xd4/0x250
[   63.816818]  arch_timer_handler_phys+0x28/0x40
[   63.816829]  handle_percpu_devid_irq+0x80/0x140
[   63.816835]  generic_handle_irq+0x24/0x38
[   63.816842]  __handle_domain_irq+0x5c/0xb0
[   63.816847]  gic_handle_irq+0x58/0xa8
[   63.816853]  el1_irq+0xb8/0x180
[   63.816860]  arch_cpu_idle+0x10/0x18
[   63.816868]  do_idle+0xcc/0x170
[   63.816875]  cpu_startup_entry+0x20/0x28
[   63.816884]  secondary_start_kernel+0x188/0x1c8




软中断：
[  176.933020] GinPot: hrtimer_tes_func.
[  176.933055] CPU: 1 PID: 0 Comm: swapper/1 Tainted: G           O      5.2.0 #20
[  176.933059] Hardware name: GinPot H5 (DT)
[  176.933064] Call trace:
[  176.933084]  dump_backtrace+0x0/0x168
[  176.933093]  show_stack+0x14/0x20
[  176.933105]  dump_stack+0xa8/0xcc
[  176.933116]  hrtimer_tes_func+0x28/0x58 [hrtimer_test]
[  176.933125]  __hrtimer_run_queues+0x120/0x1b8
[  176.933131]  hrtimer_run_softirq+0x68/0x98
[  176.933137]  __do_softirq+0x11c/0x23c
[  176.933145]  irq_exit+0xd0/0xd8
[  176.933155]  __handle_domain_irq+0x60/0xb0
[  176.933160]  gic_handle_irq+0x58/0xa8
[  176.933166]  el1_irq+0xb8/0x180
[  176.933173]  arch_cpu_idle+0x10/0x18
[  176.933182]  do_idle+0xcc/0x170
[  176.933189]  cpu_startup_entry+0x20/0x28
[  176.933198]  secondary_start_kernel+0x188/0x1c8

*/
