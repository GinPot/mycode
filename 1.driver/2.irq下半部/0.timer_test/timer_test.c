#include <linux/module.h>
#include <linux/device.h>

#include <linux/acpi.h>
#include <linux/dmi.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/i2c.h>

#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <asm/unaligned.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio.h>
#include <linux/workqueue.h>
#include <linux/of_irq.h>
#include <linux/version.h>
#include <linux/platform_device.h>

static int wait_test_flag = 0;


static struct timer_list flush_timer;
static void flush_timer_func(struct timer_list *t)
{
	wait_test_flag++;
	printk("%s: wait_test_flag=%d\n", __func__, wait_test_flag);	

	mod_timer(&flush_timer, jiffies + msecs_to_jiffies(1000));
}

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
	del_timer(&flush_timer);
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

