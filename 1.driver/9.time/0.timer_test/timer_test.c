#include <linux/module.h>
#include <linux/device.h>

/* 定时器误差在20~30ms之间(测试1秒定时，实际1.020~1.030秒之间) */

static int wait_test_flag = 0;


static struct timer_list flush_timer;
static void flush_timer_func(struct timer_list *t)					//软中断中处理,注意不要调用调度睡眠等接口，一般也再起队列处理任务
{
	wait_test_flag++;
	printk("%s: wait_test_flag=%d\n", __func__, wait_test_flag);	
	//dump_stack();

	mod_timer(&flush_timer, jiffies + msecs_to_jiffies(1000));		//中断线程中开机太早不能调用？？？	运行完这个后定时器状态就改为pending，直到调用定时器callback函数之前才会清除pending，timer_pending()可以确认callback是否执行完(虽然不是在执行完callback之后清pending)
}																	//若在中断线程中调用mod_timer()要小心系统进入睡眠的时候需要加调休不调用，防止仍然有中断信号使定时器进入pending状态，导致睡眠失败或系统crash

static int __init timer_test_init(void)
{
	//flush_timer.data = data;
	//flush_timer.function = flush_timer_func;
	//flush_timer.expires = jiffies + msecs_to_jiffies(1000);
	//setup_timer(&flush_timer, flush_timer_func, (unsigned long)&data);
	
	timer_setup(&flush_timer, flush_timer_func, 0);
	
	mod_timer(&flush_timer, jiffies + msecs_to_jiffies(1000));


	printk( "%s: wait_test_flag=%d.\n", __func__,wait_test_flag);

	return 0;
}

static void __exit timer_test_exit(void)
{
	printk( "GinPot: %s.\n", __func__);
	//del_timer(&flush_timer);										//处于pending状态的定时器会进行删除，没有在pending的定时器不会删除
	del_timer_sync(&flush_timer);									//等待callback函数运行完后再删除，不能在中断中运行(里面是死等)，更不能在这个定时器(flush_timer)的callback函数中调用，会发生死锁
}

MODULE_LICENSE("GPL");
module_init(timer_test_init);
module_exit(timer_test_exit);



//#if 如果Linux内核版本 < KERNEL_VERSION(4,14,0)
//    init_timer(&dev->getIntrTimer);
//    dev->getIntrTimer.data = (unsigned long) dev;
//    dev->getIntrTimer.function = GetIntrTimerCallback;
//    /* ... */
//    add_timer(&dev->getIntrTimer);
//#else
//    timer_setup(&dev->getIntrTimer, GetIntrTimerCallback, 0);
//    /* the third argument may include TIMER_* flags */
//    /* ... */
//#endif
//
//#if 如果Linux内核版本 < KERNEL_VERSION(4,14,0)
//void GetIntrTimerCallback(unsigned long devAddr)
//{
//    myDevice *dev = (myDevice *) devAddr;
//#else
//void GetIntrTimerCallback(struct timer_list *t)
//{
//    myDevice *dev = from_timer(dev, t, getIntrTimer);
//#endif

/*
定时器callback函数调用流程：
[   95.041920]  dump_backtrace+0x0/0x168
[   95.041938]  show_stack+0x14/0x20
[   95.041951]  dump_stack+0xa8/0xcc
[   95.041963]  flush_timer_func+0x3c/0x5c [timer_test]
[   95.041976]  call_timer_fn.isra.5+0x20/0x78
[   95.041983]  expire_timers+0xa4/0xb0					//函数处理所有的到期定时器
[   95.041990]  run_timer_softirq+0xe4/0x1a0
[   95.041997]  __do_softirq+0x11c/0x23c
[   95.042005]  irq_exit+0xd0/0xd8
[   95.042014]  __handle_domain_irq+0x60/0xb0
[   95.042020]  gic_handle_irq+0x58/0xa8
[   95.042025]  el1_irq+0xb8/0x180
[   95.042032]  arch_cpu_idle+0x10/0x18
[   95.042042]  do_idle+0xcc/0x170
[   95.042049]  cpu_startup_entry+0x24/0x28
[   95.042059]  secondary_start_kernel+0x188/0x1c8
*/

