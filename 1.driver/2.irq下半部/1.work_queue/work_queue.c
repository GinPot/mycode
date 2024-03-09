#include <linux/module.h>
#include <linux/device.h>


static int wait_test_flag = 0;

//platform_set_drvdata(pdev, di)
struct work_test {
	int cont;
	struct work_struct test_work_queue;								//不定义指针
};

struct work_test *worktest;

void work_queue_test_hardl(struct work_struct *work)
{
	struct work_test *worktest = container_of(work, struct work_test, test_work_queue)
	
	wait_test_flag++;
	printk("%s: wait_test_flag=%d\n", __func__, wait_test_flag);	
}



static int work_queue_test_init(void)
{

	printk( "%s: wait_test_flag=%d.\n", __func__,wait_test_flag);

	INIT_WORK(&test_work_queue, work_queue_test_hardl);				//初始化

	schedule_work(&test_work_queue);								//唤醒队列

	return 0;
}

static void __exit work_queue_test_exit(void)
{
	printk( "GinPot: %s.\n", __func__);
	cancel_work_sync(&test_work_queue);
	flush_work(&test_work_queue);									//移除队列
}

MODULE_LICENSE("GPL");
module_init(work_queue_test_init);
module_exit(work_queue_test_exit);
