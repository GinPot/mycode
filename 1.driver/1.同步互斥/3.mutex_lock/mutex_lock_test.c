#include <linux/module.h>
#include <linux/device.h>




static int spin_lock_test_init(void)
{
	struct mutex mutex_test;					//不能只定义指针
	mutex_init(&mutex_test);
	
	
	mutex_lock(&mutex_test);					//睡眠互斥

	pr_notice("%s: mutex lock.\n", __func__);

	mutex_unlock(&mutex_test);

	pr_notice("%s: mutex unlock.\n", __func__);

	return 0;
}

static void spin_lock_test_exit(void)
{
	pr_notice("%s\n", __func__);
	mutex_destroy()
}


MODULE_LICENSE("GPL");
module_init(spin_lock_test_init);
module_exit(spin_lock_test_exit);


/*
mutex_lock(lock) 
	if (!__mutex_trylock_fast(lock))			//假设没有互斥, 先尝试快速获取锁是否成功
		__mutex_lock_slowpath(lock)				//否则进入慢速的
			__mutex_lock(lock, TASK_UNINTERRUPTIBLE, 0, NULL, _RET_IP_)
				__mutex_lock_common(lock, state, subclass, nest_lock, ip, NULL, false)
					schedule_preempt_disabled()
						schedule()

mutex_unlock(lock) 
	__mutex_unlock_slowpath(lock, _RET_IP_)
		struct mutex_waiter *waiter = list_first_entry(&lock->wait_list, struct mutex_waiter, list)			//从互斥锁结构体的wait_list链表中取出witer的线程task_trace结构体，加入到等待队列中wake_q
		wake_q_add(&wake_q, next)
		wake_up_q(&wake_q)
			wake_up_process(task)																			//唤醒所有因互斥锁睡眠的线程

*/






