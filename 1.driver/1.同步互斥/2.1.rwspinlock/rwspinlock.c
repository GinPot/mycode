#include <linux/module.h>
#include <linux/device.h>


static DEFINE_RWLOCK(rwspinlock_test);		//初始化方案1

static int rwspinlock_test_init(void)
{
//	rwlock_t rwspinlock_test;					//不能只定义指针
//	rwlock_init(&rwspinlock_test);				//初始化方案2
	
	
	read_lock(&rwspinlock_test);

	pr_notice("%s: swspin lock.\n", __func__);

	read_unlock(&rwspinlock_test);

	pr_notice("%s: swspin unlock.\n", __func__);

	return 0;
}

static void rwspinlock_test_exit(void)
{
	pr_notice("%s\n", __func__);
}


MODULE_LICENSE("GPL");
module_init(rwspinlock_test_init);
module_exit(rwspinlock_test_exit);


/*
Read/Write spin lock(读写自旋锁): 	
	1、允许多个reader并发执行,但reader和writer不能并发执行，更多照顾reader：
	2、若reader先在执行，后writer等待，后面又有reader进来会将writer等待的时间延长，所有reader执行完后writer才进入
	2、自旋等待

注： 内核不鼓励使用rwspinlock,RCU是更好的选择！


typedef struct {
    u32 lock;
} arch_rwlock_t;

	 -------------------------------------------------------------------------------
	|  write thread count  | 				Read thread counter						|
	 -------------------------------------------------------------------------------
	31                     30                                                        0

32个bit的lock，0～30的bit用来记录进入临界区的read thread的数目，第31个bit用来记录write thread的数目




================

定义rw spin lock并初始化											DEFINE_RWLOCK
				
动态初始化rw spin lock											rwlock_init
				
获取指定的rw spin lock											read_lock
																write_lock
																
获取指定的rw spin lock同时disable本CPU中断							read_lock_irq
																write_lock_irq
												
保存本CPU当前的irq状态，disable本CPU中断并获取指定的rw spin lock		read_lock_irqsave
																write_lock_irqsave
获取指定的rw spin lock同时disable本CPU的bottom half				read_lock_bh
																write_lock_bh

=============================

释放指定的spin lock												read_unlock
																write_unlock
																
释放指定的rw spin lock同时enable本CPU中断							read_unlock_irq
																write_unlock_irq
																
释放指定的rw spin lock同时恢复本CPU的中断状态						read_unlock_irqrestore
																write_unlock_irqrestore
																
获取指定的rw spin lock同时enable本CPU的bottom half					read_unlock_bh
																write_unlock_bh
																
尝试去获取rw spin lock，如果失败，不会spin，而是返回非零值				read_trylock
																write_trylock



*/