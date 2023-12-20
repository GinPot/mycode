#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/kthread.h>


struct task_struct *waiteventthread1;
struct task_struct *waiteventthread2;


static wait_queue_head_t test_que;							//定义等待队列的结构体
static int block_flag=0;


static int waitevent_thread1(void *data)
{

	pr_notice("start wait_event\n");
	
    wait_event(test_que, block_flag);						//睡眠等待
	block_flag = 0;

    pr_notice("after wait_event\n");


	
	return 0;
}

static int waitevent_thread2(void *data)
{
	pr_notice("start222 waitevent_thread1\n");
	
    //wait_event(test_que, block_flag);						//睡眠等待
	//block_flag = 0;
  
    pr_notice("after222 waitevent_thread1\n");


	return 0;
}

static int waitevent_test_init(void)
{
    init_waitqueue_head(&test_que);             			//初始化wait_queue_head_t变量

	waiteventthread1 = kthread_run(waitevent_thread1, NULL, "waitevent_thread1");
	if (IS_ERR(waiteventthread1)) {
		pr_err("%s: creat rcu_reader_list thread failed\n", __func__);
		return IS_ERR(waiteventthread1);
	}
	waiteventthread2 = kthread_run(waitevent_thread2, NULL, "waitevent_thread2");
	if (IS_ERR(waiteventthread2)) {
		pr_err("%s: creat rcu_reader_list thread failed\n", __func__);
		return IS_ERR(waiteventthread2);
	}

	pr_notice("%s: init success. \n", __func__);

	msleep(5000);
	block_flag = 1;
	wake_up(&test_que);										//可同是唤醒多个线程的wait_evnet,只要条件满足；若wake_up后面的block_flag条件还是不满足的话仍会继续睡眠

	
	return 0;
}

static void waitevent_test_exit(void)
{
	pr_notice("%s\n", __func__);
}


MODULE_LICENSE("GPL");
module_init(waitevent_test_init);
module_exit(waitevent_test_exit);


/*
等待队列


wait_event_interruptible_timeout(test_que, block_flag, msecs_to_jiffies(1000))
wait_event_interruptible(test_que, block_flag)
wake_up_interruptible(&test_que)


wait_event_timeout(test_que, block_flag, msecs_to_jiffies(1000))
wake_up(&test_que)



若系统广播试的信号量需要告诉整个系统，这里有可以用wait_event_interruptible()去打断，如reboot需要关机时，若不能被打断的wait_event()，
关机流程就会卡在这如法走下去，在需要高频等待中，建议用可以打断的等待。
*/


/*
init_waitqueue_head(&test_que);
	__init_waitqueue_head((test_que), #wq_head, &__key)
		spin_lock_init(&wq_head->lock)								//初始化等待队列的自旋锁
		INIT_LIST_HEAD(&wq_head->head)								//初始化等待队列的链表头


wait_event(test_que, block_flag)
	if (block_flag)													//检测条件是否满足
		break；
	__wait_event(wq_head, block_flag)
		___wait_event(wq_head, block_flag, TASK_UNINTERRUPTIBLE, 0, 0, schedule())
			init_wait_entry(&__wq_entry, exclusive ? WQ_FLAG_EXCLUSIVE : 0)			//初始化等待队列里的成员信息
				wq_entry->flags = 0
				wq_entry->func = autoremove_wake_function
				INIT_LIST_HEAD(&wq_entry->entry)
			for (;;)
				prepare_to_wait_event(&wq_head, &__wq_entry, state)
					__add_wait_queue(wq_head, wq_entry)				//将需要等待的entry加入到等待队列的链表中
						wq_entry->flags |= WQ_FLAG_EXCLUSIVE
				if (block_flag)										//唤醒后再次检测条件是否满足，满足则跳出循环
					break;
				
				schedule()											//非抢占调度
			finish_wait(&wq_head, &__wq_entry)


wake_up(&test_que)
	__wake_up(x, TASK_NORMAL, 1, NULL)
		__wake_up_common_lock(wq_head, mode, nr_exclusive, 0, key)
			 __wake_up_common(wq_head, mode, nr_exclusive, wake_flags, key, &bookmark)		//nr_exclusive=0是唤醒所有entry，非0则唤醒1个具体的entry
				curr = list_first_entry(&wq_head->head, wait_queue_entry_t, entry)			//获取等待队列链表第一个entry
				if (&curr->entry == &wq_head->head)											//等于则链表为空
					return;
				list_for_each_entry_safe_from(curr, next, &wq_head->head, entry)			//一次获去等待队列链表上的entry，
					curr->func(curr, mode, wake_flags, key)
						autoremove_wake_function(curr, mode, wake_flags, key)				// 888
							default_wake_function(wq_entry, mode, sync, key)
								try_to_wake_up(curr->private, TASK_NORMAL, 0)
							list_del_init(&wq_entry->entry);								//从等待队列中删除
					if (ret && (flags & WQ_FLAG_EXCLUSIVE) && !--nr_exclusive)				//正常情况下下ret=1,有WQ_FLAG_EXCLUSIVE标志位，nr_exclusive为1,减1后为0
						break																//所以正常情况下这里就退出去了
						
*/











