#include <linux/module.h>
#include <linux/device.h>


//static DEFINE_SPINLOCK(spin_locktest);		//初始化方案1

static int spin_lock_test_init(void)
{
	spinlock_t spin_locktest;					//不能只定义指针
	spin_lock_init(&spin_locktest);				//初始化方案2
	
	
	spin_lock(&spin_locktest);

	pr_notice("%s: spin lock.\n", __func__);

	spin_unlock(&spin_locktest);

	pr_notice("%s: spin unlock.\n", __func__);

	return 0;
}

static void spin_lock_test_exit(void)
{
	pr_notice("%s\n", __func__);
}


MODULE_LICENSE("GPL");
module_init(spin_lock_test_init);
module_exit(spin_lock_test_exit);


/*
spin lock: 				是互斥的，任何时候只有一个thread（reader or writer）进入临界区;
Read/Write spin lock: 	允许多个reader并发执行,但reader和writer不能并发执行，更多照顾reader；
seq lock： 				允许多个reader并发执行,但reader和writer不能并发执行，更多照顾writer；


(1)void spin_lock(spinlock_t *lock);									//申请自旋锁，如果锁被其他处理器占有，当前处理器自旋等待。
(2)void spin_lock_bh(spinlock_t *lock);									//申请自旋锁，并且禁止当前处理器的软中断。
(3)void spin_lock_irq(spinlock_t *lock);								//申请自旋锁，并且禁止当前处理器的硬中断。
(4)spin_lock_irqsave(lock, flags);										//申请自旋锁，保存当前处理器的硬中断状态，并且禁止当前处理器的硬中断。
(5)int spin_trylock(spinlock_t *lock);									//申请自旋锁，如果申请成功，返回1；如果锁被其他处理器占有，当前处理器不等待，立即返回0。

...

(1)void spin_unlock(spinlock_t *lock);
(2)void spin_unlock_bh(spinlock_t *lock);								//释放自旋锁，并且开启当前处理器的软中断。
(3)void spin_unlock_irq(spinlock_t *lock);								//释放自旋锁，并且开启当前处理器的硬中断。
(4)void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);	//释放自旋锁，并且恢复当前处理器的硬中断状态。
*/