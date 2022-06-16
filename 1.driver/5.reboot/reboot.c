#include <linux/module.h>
#include <linux/device.h>
#include <linux/reboot.h>
#include <linux/delay.h>

static int reboot_init(void)
{

	printk("%s\n", __func__);
	
	msleep(5000);
	kernel_restart("virtio_net virtio1 error, reboot android!");


	return 0;
}

static void reboot_exit(void)
{
	printk("%s\n", __func__);

}

MODULE_LICENSE("GPL");
module_init(reboot_init);
module_exit(reboot_exit);