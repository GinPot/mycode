//lsmod名字和ko名字一样


#include <linux/module.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include <linux/fs.h>
#include <linux/cdev.h>

#include <linux/ioctl.h>

#include <linux/mm.h>
#include <linux/slab.h>


#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/poll.h>

#define DEV_MAJOR	0					//设置0，即用动态分配的
#define DEV_MINOR	0
#define DEV_NAME	"driver_test"		//该name好像没啥用


static dev_t devt;
static struct cdev *cdev;
static struct class *driverClass;
static struct device *driverDev;



#define IOC_MAGIC 'c'					/*定义设备类型*/
#define IOCINIT _IO(IOC_MAGIC, 0)		/* 初始化设备*/
#define IOCWREG _IOW(IOC_MAGIC, 1, int)	/* 写寄存器 */
#define IOCRREG _IOR(IOC_MAGIC, 2, int)	/* 读寄存器 */
#define IOC_MAXNR 2


//DECLARE_WAIT_QUEUE_HEAD(pollwait);		//注册一个等待队列pollwait
static wait_queue_head_t pollwait;
static int pollFlag = 0;


static struct fasync_struct *fasync;			//定义fasync_struct结构

//struct TestData {
//	
//};
static char *mmapbuf=NULL;
static int DriverTestOpen(struct inode *inode, struct file *file)
{
	pr_notice("%s: enter\n", __func__);

	mmapbuf = kzalloc(4096, GFP_KERNEL);
	if (!mmapbuf)
		return -ENOMEM;
	
	memset(mmapbuf, 0x88, 4096);

	init_waitqueue_head(&pollwait);
			
	return 0;
}

static ssize_t DriverTestRead(struct file *file, char __user *user_buf, size_t size, loff_t *ppos)
{
	char readbuf[] = "read buf.";

	pr_notice("%s: size:%lu readbuf, %s\n", __func__, sizeof(readbuf), readbuf);
	
	if(copy_to_user(user_buf, readbuf, sizeof(readbuf))){
		pr_err("%s: failed to copy to user cfg_data.\n", __func__);
		return -EIO;
	}
	
	return sizeof(readbuf);			//return的值即为read()返回的值
}

static ssize_t DriverTestWrite(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	char writebuf[100];

	
	if (copy_from_user(writebuf, user_buf, count)) {
		pr_err("%s: failed to copy from user cfg_data.\n", __func__);
		return -EIO;
	}
	
	pr_notice("%s: size:%lu writebuf, %s\n", __func__, count, writebuf);
	
	return count;				//return的值即为write()返回的值
}


static long DriverTestIoctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	char writebuf[100];
	char readbuf[] = "ioctl read buf.";
	
	pr_notice("%s: enter\n", __func__);

	if(_IOC_TYPE(cmd) != IOC_MAGIC) {			//如果设备种类不正确
		pr_err("[%s] command type [%c] error!\n", __func__, _IOC_TYPE(cmd));
		return -1;
	}
	if(_IOC_NR(cmd) > IOC_MAXNR ) {				//检查序号是否超限
		pr_err("[%s] command number [%d] exceeded!\n", __func__, _IOC_NR(cmd));
		return -1;
	}


	switch(cmd)
	{
		//初始化
		case IOCINIT:
			pr_notice("hello, i am register!\n");
		break;

		//写数据
		case IOCWREG:
			ret = copy_from_user(writebuf, (int __user *)arg, 12);
			pr_notice("%s:  writebuf, %s\n", __func__, writebuf);
		break;

		//读数据
		case IOCRREG:
			ret = copy_to_user((int __user *)arg, readbuf, sizeof(readbuf));
		break;

		default: break;
	}

	return ret;
}

static int DriverTestMmap(struct file *file, struct vm_area_struct *vma)
{
	int ret;
	
	pr_notice("data: 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x", mmapbuf[0],mmapbuf[1],mmapbuf[2],mmapbuf[3],mmapbuf[4],mmapbuf[5],mmapbuf[6],mmapbuf[7],mmapbuf[8],mmapbuf[9]);
	pr_notice("vma->vm_start: 0x%08lx    len:%lu\n", vma->vm_start, vma->vm_end - vma->vm_start);
	
	//vma->vm_start用户称不指定的话则系统自己选择虚拟地址，vma->vm_end - vma->vm_start，大小由用户空间指定,需要大于4K， 物理地址因需要右移一页大小，所以物理地址需要大于4K
	ret = remap_pfn_range(vma, vma->vm_start, virt_to_phys(mmapbuf) >> PAGE_SHIFT, vma->vm_end - vma->vm_start, vma->vm_page_prot);
	if (ret) {
		pr_err("Remapping remap_pfn_range memory, error: %d\n", ret);
		return ret;
	}
	
	return ret;
}

static __poll_t DriverTestPoll(struct file *file, struct poll_table_struct *wait)
{
	__poll_t masks = 0;
	
	pr_notice("%s: enter \n", __func__);
	
	poll_wait(file, &pollwait, wait);		//wait_event_interruptible(pollwait, pollFlag);//将当前进程放入等待队列pollwait中
	if(pollFlag){
		pollFlag = 0;
		masks |= POLLIN | POLLRDNORM;
	}
	
	
	return masks;
}

static int DriverTestFasync(int fd, struct file *file, int on)
{
	int ret;
	
	pr_notice("%s: enter \n", __func__);
	
	ret = fasync_helper(fd, file, on, &fasync);			//利用fasync_helper初始化fifth_fasync
    if (ret < 0)
        return ret;

    return 0;
}

static int DriverTestRelease(struct inode *inode, struct file *file)
{
	pr_notice("%s: close\n", __func__);
	
	
	return 0;
}

static const struct file_operations fops = {
	.owner 			= THIS_MODULE,
	.open 			= DriverTestOpen,
	.read 			= DriverTestRead,
	.write 			= DriverTestWrite,
	.unlocked_ioctl = DriverTestIoctl,
	.mmap			= DriverTestMmap,
	.poll			= DriverTestPoll,
	.fasync			= DriverTestFasync,
	.release 		= DriverTestRelease,
};


static char sysBuf[10]={0};
static ssize_t DriverSysGetValue(struct device *dev, struct device_attribute *attr, char *buf)
{
	kill_fasync(&fasync, SIGIO, POLL_IN);			//发生信号给进程
	
	return sprintf(buf, "%s", sysBuf);
}

static ssize_t DriverSysSetValue(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	pollFlag = 1;
	wake_up_interruptible(&pollwait);   /* 唤醒休眠的进程 */
	
	return sprintf(sysBuf, "%s", buf);
}

static DEVICE_ATTR(syschar, S_IRUGO | S_IWUSR, DriverSysGetValue, DriverSysSetValue);

static int DriverTestInit(void)
{
	int ret=0;


	pr_notice("%s: enter\n", __func__);

	devt = MKDEV(DEV_MAJOR, DEV_MINOR);
	if(!MAJOR(devt))
		ret = alloc_chrdev_region(&devt, MINOR(devt), 1, DEV_NAME);					//动态生成设备号，并在内核中登记;第二个参数是次设备号从几开始，第三个参数是次设备号的个数
	else
		ret = register_chrdev_region(devt, 1, DEV_NAME);							//若有指定主设备号，则用指定的
	if(ret < 0){
		pr_err("%s: fail to register chrdev region.\n", __func__);
		return ret;
	}
	pr_notice("%s: MAJOR=%d, MINOR=%d\n", __func__, MAJOR(devt), MINOR(devt));

	cdev = cdev_alloc();
	if(!cdev){
		pr_err("%s: fail to ralloc cdev.\n", __func__);
		ret = -ENOMEM;
		goto err_chrdev;
	}
	
	cdev->owner = THIS_MODULE;
	cdev->ops = &fops;
	
	ret = cdev_add(cdev, devt, 1);													//把主从设备号添加到内核链表中，以便后续系统调用通过主次设备号找到对应的fops
	if(ret){
		pr_err("%s: fail to add cdev.\n", __func__);
		goto err_cdev;
	}

/******************************************************************************/	

	driverClass = class_create(THIS_MODULE, "driverClass");							//kset_register创建/sys/class/driverClass/目录
	if(IS_ERR(driverClass)){														//创建class不是创建节点的重点，只是为下面函数提供参数
		pr_err("%s: fail to class create.\n", __func__);
		ret = PTR_ERR(driverClass);
		goto err_cdev;
	}

	driverDev = device_create(driverClass, NULL, devt, NULL, "%s","driverTest");	//1、指定主从设备好devt创建device及/dev/driverTest节点
	if(IS_ERR(driverDev)){															//2、是否会创建dev节点的关键：是否会传入主次设备号devt;传入MKDEV(0, 0)即不会生成设备节点,不能为NULL
		pr_err("%s: fail to device create.\n", __func__);							//3、第二个参数NULL，即没有父设备；将会再/sys/devices/virtual/driverClass/创建对应的设备
		ret = PTR_ERR(driverDev);
		goto err_device;
	}

/******************************************************************************/

	ret = device_create_file(driverDev, &dev_attr_syschar);							//在/sys/devices/virtual/driverClass/driverTest/下面创建syschar文件
	if(ret < 0){
		pr_err("%s: fail to create sys file.\n", __func__);
		goto err_sys;
	}

	return ret;

err_sys:
	device_destroy(driverClass, devt);
err_device:
	class_destroy(driverClass);
err_cdev:
	cdev_del(cdev);
err_chrdev:
	unregister_chrdev_region(devt, 1);
	return ret;
}

static void DriverTestExit(void)
{
	pr_notice("%s\n", __func__);

	device_remove_file(driverDev, &dev_attr_syschar);
	device_destroy(driverClass, devt);
	class_destroy(driverClass);
	cdev_del(cdev);
	unregister_chrdev_region(devt, 1);												//第二个参数为次设备号个数
}

MODULE_LICENSE("GPL");
module_init(DriverTestInit);
module_exit(DriverTestExit);



/*
大内核锁(BKL):
	早期linux版本对对称多处理(SMP)器的支持非常有限，为了保证可靠性，对处理器之间的互斥采取了‘宁可错杀三千，不可放过一个’的方式：在内核入口处安装一把‘巨大’的锁，一旦一个处理器进入内核态就立刻上锁，其它将要进入内核态的进程只能在门口等待，
	以此保证每次只有一个进程处于内核态运行。这把锁就是大内核锁。有了大内核锁保护的系统当然可以安全地运行在多处理器上：由于同时只有一个处理器在运行内核代码，内核的执行本质上和单处理器没有什么区别；而多个处理器同时运行于进程的用户态也是安全的，
	因为每个进程有自己独立的地址空间。但是这样粗鲁地加锁其缺点也是显而易见的：多处理器对性能的提示只能体现在用户态的并行处理上，而在内核态下还是单线执行，完全无法发挥多处理器的威力
	https://www.iteye.com/blog/java4gis-947773

unlocked_ioctl替代ioctl原因：
	用户在调用ioctl进入内核态时，原来的ioctl受到大内核锁的保护(多核的情况下只能单核运行ioctl)；unlocked_ioctl则直接运行，并发访问时需要驱动自己加锁


*/