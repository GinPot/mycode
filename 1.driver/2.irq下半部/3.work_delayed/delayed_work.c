#include <linux/module.h>
#include <linux/device.h>


static int wait_test_flag = 0;

struct delayed_work test_work;


void delayed_work_test_hardl(struct work_struct *work)
{
	wait_test_flag++;
	printk("%s: wait_test_flag=%d\n", __func__, wait_test_flag);	
	
	schedule_delayed_work(&test_work, usecs_to_jiffies(1*1000*1000));
}



static int __init wait_test_init(void)
{

	printk( "GinPot: %s.\n", __func__);

	INIT_DELAYED_WORK(&test_work, (void *)delayed_work_test_hardl);


	schedule_delayed_work(&test_work, usecs_to_jiffies(1*1000*1000));
	
	printk( "%s: wait_test_flag=%d.\n", __func__,wait_test_flag);

	return 0;
}

static void __exit wait_test_exit(void)
{
	printk( "GinPot: %s.\n", __func__);
	cancel_delayed_work_sync(&test_work);
	flush_delayed_work(&test_work);
}

MODULE_LICENSE("GPL");
module_init(wait_test_init);
module_exit(wait_test_exit);
