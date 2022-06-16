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


static struct input_dev *virt_touch_input;     //新建一个输入子系统的设备层结构
static struct class  *virt_touch_class;
static struct device *virt_touch_dev;
int major;


int virt_touch_open (struct inode *inode, struct file *file)
{
	printk("virt_touch_open\n");
	return 0;
}


ssize_t virt_touch_write (struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
	unsigned char val[5], ret;
	int buf_vir[3];
	
	ret = copy_from_user(val, buf, 5);
	buf_vir[0] = (val[0] << 8) | val[1];
	buf_vir[1] = (val[2] << 8) | val[3];
	buf_vir[2] = val[4];
	printk("%d   %d   %d   %d   %d\n",val[0],val[1],val[2],val[3],val[4]);
	printk("buf_vir[0]=%d  buf_vir[1]=%d buf_vir[2]=%d\n",buf_vir[0],buf_vir[1],buf_vir[2]);

    input_report_abs(virt_touch_input,ABS_X,buf_vir[0]);
    input_report_abs(virt_touch_input,ABS_Y,buf_vir[1]);
    input_report_abs(virt_touch_input,ABS_PRESSURE,buf_vir[2]);	//压力值为1
    input_report_key(virt_touch_input,BTN_TOUCH,val[2]);     //1表示按下
    input_sync(virt_touch_input);//事件已经处理完毕

	return size;
}



static struct file_operations virt_touch_fops = {
    .owner   =  THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open    =  virt_touch_open,     
	.write	 =	virt_touch_write,	   
};


static int virt_touch_init(void)
{
	int ret;
	printk("virt_touch_init\n");
	
    /* 1、分配一个input_dev结构体 */
    virt_touch_input = input_allocate_device();//在设备层分配一个input_dev结构
    if (!virt_touch_input)
        return -ENOMEM;
    
    /* 2、设置它 */
        /* 2.1 能产生哪些事件 */
    __set_bit(EV_KEY, virt_touch_input->evbit); //按键事件
    __set_bit(EV_ABS, virt_touch_input->evbit);//绝对位移事件
    __set_bit(EV_SYN, virt_touch_input->evbit);	
            
        /* 2.2 能产生这类事件里的哪些事件 */
    __set_bit(BTN_TOUCH, virt_touch_input->keybit);//按键事件里的BTN_TOUCH事件
       
    input_set_abs_params(virt_touch_input, ABS_X, 0, 1920, 0, 0);
    input_set_abs_params(virt_touch_input, ABS_Y, 0, 1080, 0, 0);
    input_set_abs_params(virt_touch_input, ABS_PRESSURE, 0, 1, 0, 0);

    /* 3、注册 */
	ret = input_register_device(virt_touch_input);//注册input设备驱动 
	if (ret) {
		input_free_device(virt_touch_input);
		return ret;
	}

	major = register_chrdev(0, "virt_touch", &virt_touch_fops);
	virt_touch_class = class_create(THIS_MODULE, "virt_touch");

	/* 为了让mdev根据这些信息来创建设备节点 */
	virt_touch_dev = device_create(virt_touch_class, NULL, MKDEV(major, 0), NULL, "virt_touch"); /* /dev/buttons */

	return 0;
}



static void virt_touch_exit(void)
{
	printk("virt_touch_exit\n");
	input_unregister_device(virt_touch_input);//反注册
    input_free_device(virt_touch_input);//释放分配的input_dev结构	

	unregister_chrdev(major, "virt_touch");
	device_unregister(virt_touch_dev);
	class_destroy(virt_touch_class);	
}

module_init(virt_touch_init);
module_exit(virt_touch_exit);

MODULE_LICENSE("GPL");