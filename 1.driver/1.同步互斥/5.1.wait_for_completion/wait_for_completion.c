#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>

struct task_struct *waitforthread1;
struct task_struct *waitforthread2;

static struct completion comple;



static int waitfor_thread1(void *data)
{

	pr_notice("start wait_for_completion\n");
	
    wait_for_completion(&comple);						//睡眠等待
  
    pr_notice("after wait_for_completion\n");


	
	return 0;
}

static int waitfor_thread2(void *data)
{
	pr_notice("start222 wait_for_completion\n");
	
    wait_for_completion(&comple);						//睡眠等待
  
    pr_notice("after222 wait_for_completion\n");
             									
	//reinit_completion(&mw->mw_complete);				//Re-arm the completion in case we want to wait on it again

	return 0;
}

static int waitfor_test_init(void)
{
    init_completion(&comple);             //初始化completion变量

	waitforthread1 = kthread_run(waitfor_thread1, NULL, "waitfor_thread1");
	if (IS_ERR(waitforthread1)) {
		pr_err("%s: creat rcu_reader_list thread failed\n", __func__);
		return IS_ERR(waitforthread1);
	}
	waitforthread2 = kthread_run(waitfor_thread2, NULL, "waitfor_thread2");
	if (IS_ERR(waitforthread2)) {
		pr_err("%s: creat rcu_reader_list thread failed\n", __func__);
		return IS_ERR(waitforthread2);
	}

	pr_notice("%s: init success. \n", __func__);

	msleep(5000);
	complete(&comple);    						//waitfor_thread1先睡，则改线程先唤醒
	msleep(5000);
	complete(&comple); 

	//msleep(5000);
	//complete_all(&comple);					//可以唤醒所有该comple上的所有等待线程,运行后如还需要继续使用需要reinit_completion()下

	
	return 0;
}

static void waitfor_test_exit(void)
{
	pr_notice("%s\n", __func__);
}


MODULE_LICENSE("GPL");
module_init(waitfor_test_init);
module_exit(waitfor_test_exit);


/*
完成量,依赖等待队列


wait_for_completion_interruptible_timeout(&comple, msecs_to_jiffies(1000))
wait_for_completion_timeout(&comple, msecs_to_jiffies(1000))			//超时1s等待
wait_for_completion_interruptible(&comple)								//可以被signal打断
complete(&comple)

wait_for_completion_killable(&comple)									//Ctrl+C强制进程退出睡眠，通过返回值确认是正常退出还是异常退出
wait_for_completion_killable_timeout(&comple)
complete(&comple)


若系统广播试的信号量需要告诉整个系统，这里有可以用wait_event_interruptible()去打断，如reboot需要关机时，若不能被打断的wait_event()，
关机流程就会卡在这如法走下去，在需要高频等待中，建议用可以打断的等待。
*/


/*
wait_for_completion
	wait_for_common(x, MAX_SCHEDULE_TIMEOUT, TASK_UNINTERRUPTIBLE)
		__wait_for_common(x, schedule_timeout, timeout, state)
			do_wait_for_common(x, action, timeout, state)
				if (!x->done)											//空闲时done都是0，还会在complete中+1
					DECLARE_WAITQUEUE(wait, current);					//将当前线程加入到wait的struct wait_queue_entry结构体中
					_add_wait_queue_entry_tail_exclusive(&x->wait, &wait)		//把wait加入到completion->wait等待队列的链表中
					timeout = action(timeout) ==> schedule_timeout(timeout)		//调度
						schedule()												//非抢占调度
					if (x->done != UINT_MAX)
						x->done--												//走到这是因为有人唤醒或者超时，把之前别人+1的减回去
					



complete
	x->done++
	__wake_up_locked(&x->wait, TASK_NORMAL, 1)
		__wake_up_common(wq_head, mode, nr, 0, NULL, NULL);
			list_for_each_entry_safe_from(curr, next, &wq_head->head, entry)	//取completion->wait等待队列的链表中第一wait_queue_entry结构体
				curr->func(curr, mode, wake_flags, key)	==>	default_wake_function
					try_to_wake_up(curr->private, mode, wake_flags)
	
*/











