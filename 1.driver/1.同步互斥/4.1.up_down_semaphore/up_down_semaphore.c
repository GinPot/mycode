#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>

struct task_struct *rcu_reader;
struct task_struct *rcu_updater;

struct semaphore sema;

static int UpDown_thread1(void *data)
{
	int ret;

	pr_notice("start down_interruptible, sema.count: %d\n", sema.count);
	
    ret = down_interruptible(&sema);							//因为初始化为0，计数-1, 再减则会进入睡眠等待

    
    pr_notice("after down_interruptible, sema.count: %d, ret=%d\n", sema.count, ret);


	
	return 0;
}


static int UpDown_thread2(void *data)
{
	msleep(5000);
	up(&sema);                 									//释放信号量，计数+1，唤醒线程UpDown_thread1

	return 0;
}


static int UpDown_test_init(void)
{
    sema_init(&sema, 0);                						//信号量初始化

	rcu_reader = kthread_run(UpDown_thread1, NULL, "UpDown_thread1");
	if (IS_ERR(rcu_reader)) {
		pr_err("%s: creat rcu_reader_list thread failed\n", __func__);
		return IS_ERR(rcu_reader);
	}

	rcu_updater = kthread_run(UpDown_thread2, NULL, "UpDown_thread2");
	if (IS_ERR(rcu_updater)) {
		pr_err("%s: creat rcu_updater_list thread failed\n", __func__);
		return IS_ERR(rcu_updater);
	}

	pr_notice("%s: init success. \n", __func__);

	return 0;
}

static void UpDown_test_exit(void)
{
	pr_notice("%s\n", __func__);
}


MODULE_LICENSE("GPL");
module_init(UpDown_test_init);
module_exit(UpDown_test_exit);

/*
down_interruptible
	__down_interruptible(sem)
		__down_common(sem, TASK_INTERRUPTIBLE, MAX_SCHEDULE_TIMEOUT)
			__set_current_state(TASK_INTERRUPTIBLE)
			schedule_timeout(timeout)
				schedule()										//睡眠等待

up()
	__up(sem)
		waiter = list_first_entry(&sem->wait_list, struct semaphore_waiter, list)
		wake_up_process(waiter->task)							//唤醒睡眠的线程




tatic int UpDown_test_init(void)
{

    int ret;
    sema_init(&sema, 5);                						//信号量初始化

    //输出初始化后信号量的信息 
    pr_notice("after sema_init, sema.count: %d\n", sema.count);
    ret = down_interruptible(&sema);     						//获取信号量

    //输出down_interruptible操作后信号量的信息
    pr_notice("first down_interruptible, ret = %d\n", ret);    	//输出返回值

    // 输出信号量sema的自旋锁字段的值
    pr_notice("first down_interruptible, sema.count: %d\n", sema.count);	// 输出信号量计数器值

    sema_init(&sema, 0);
    ret = down_interruptible( &sema);							//因为上面初始化为0，再减则会进入睡眠等待

    pr_notice("second down_interruptible, ret = %d\n", ret);	//返回 -EINTR
    pr_notice("second down_interruptible, sema.count: %d\n", sema.count);

	return 0;
}


首先定义一个信号量结构体sema，并调用函数sema_init()初始化该信号量，将其计数器设置为5。
第一次调用down_interruptible()获取信号量，其计数器count将减1而变为4，同时该函数的返回值为0，即成功获取了信号量。
然后调用函数sema_init()将信号量sema的计数器值设为0，此后再调用down_interruptible()进程将处于等待获取信号量的状态，
这时可向该进程发送一个信号使其放弃获取信号量而返回。这里测试时，通过键盘命令Ctrl+z发送一个从键盘退出的信号，
进程将立即返回且返回值为-EINTR，由输出信息可知，该值为-4

*/
