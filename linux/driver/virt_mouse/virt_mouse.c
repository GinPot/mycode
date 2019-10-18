#include <linux/atomic.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/input.h>
#include <linux/input-polldev.h>
#include <linux/slab.h>
#include <linux/hid.h>


static struct input_dev *virt_mouses_input;     //新建一个输入子系统的设备层结构
static struct class  *virt_mouses_class;
static struct device *virt_mouses_dev;
static int major;


int virt_mouses_open (struct inode *inode, struct file *file)
{
	//printk("virt_mouses_open\n");
	return 0;
}

ssize_t virt_mouses_write (struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
	int val[3], ret;
	
	ret = copy_from_user(val, buf, size);

	//printk("%d   %d   %d  size=%d\n",val[0],val[1],val[2],size);
	
    input_report_rel(virt_mouses_input,REL_X,val[0]);
    input_report_rel(virt_mouses_input,REL_Y,val[1]);
    input_report_key(virt_mouses_input,BTN_LEFT,val[2]);     //1表示按下
    input_sync(virt_mouses_input);//事件已经处理完毕

	return size;
}

static struct file_operations virt_mouses_fops = {
    .owner   =  THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open    =  virt_mouses_open,     
	.write	 =	virt_mouses_write,	   
};

static int virt_mouses_init(void)
{
	int ret;
	//printk("virt_mouses_init\n");
	
    /* 1、分配一个input_dev结构体 */
    virt_mouses_input = input_allocate_device();//在设备层分配一个input_dev结构
    if(!virt_mouses_input)
    {
        return -ENOMEM;
    }

	virt_mouses_input->name = "virt_mouses";
	virt_mouses_input->phys = "virt_mouses_none";	
	
	virt_mouses_input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
	virt_mouses_input->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) |
		BIT_MASK(BTN_RIGHT) | BIT_MASK(BTN_MIDDLE);
	virt_mouses_input->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y);
	virt_mouses_input->keybit[BIT_WORD(BTN_MOUSE)] |= BIT_MASK(BTN_SIDE) |
		BIT_MASK(BTN_EXTRA);
	virt_mouses_input->relbit[0] |= BIT_MASK(REL_WHEEL);

    /* 3、注册 */
	ret = input_register_device(virt_mouses_input);//注册input设备驱动 
	if (ret) {
		input_free_device(virt_mouses_input);
		return ret;
	}

	major = register_chrdev(0, "virt_mouses", &virt_mouses_fops);
	virt_mouses_class = class_create(THIS_MODULE, "virt_mouses");

	/* 为了让mdev根据这些信息来创建设备节点 */
	virt_mouses_dev = device_create(virt_mouses_class, NULL, MKDEV(major, 0), NULL, "virt_mouses"); /* /dev/buttons */

	return 0;
}

static void virt_mouses_exit(void)
{
	//printk("virt_mouses_exit\n");
	input_unregister_device(virt_mouses_input);//反注册
    input_free_device(virt_mouses_input);//释放分配的virt_mouses_input结构	

	unregister_chrdev(major, "virt_mouses");
	device_unregister(virt_mouses_dev);
	class_destroy(virt_mouses_class);	
}

module_init(virt_mouses_init);
module_exit(virt_mouses_exit);

MODULE_LICENSE("GPL");