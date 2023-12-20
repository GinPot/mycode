#include <linux/delay.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kthread.h>


struct task_struct *pthread_reciver;

static int kthread_test(void *data)
{
	int kthread_flag = 0;

	while (!kthread_should_stop()){								//调用kthread_stop后会退出
		kthread_flag++;
		printk("%s: kthread_flag=%d\n", __func__, kthread_flag);
		msleep(1000);
		try_to_freeze();										//在系统进入str时会唤醒内核线程，运行到这就会freeze改线程；若没有这样的调用的内核线程则不会freeze
	}

	pr_notice("%s: exit with not we want\n", __func__);

	return 0;
}

static int driver_kthread_init(void)
{
	pr_notice("%s: enter\n", __func__);
	
	pthread_reciver = kthread_run(kthread_test, NULL, "kthread_test");
	if (IS_ERR(pthread_reciver)) {
		pr_notice("%s: creat thread failed\n", __func__);
		return IS_ERR(pthread_reciver);
	}

	pr_notice("%s: kthread_run ok\n", __func__);

	return 0;
}

static void driver_kthread_exit(void)
{
	pr_notice("%s\n", __func__);
	kthread_stop(pthread_reciver);		//在调用kthread_stop函数时，线程函数不能已经运行结束。否则，kthread_stop函数会一直进行等待
}

MODULE_LICENSE("GPL");
module_init(driver_kthread_init);
module_exit(driver_kthread_exit);
