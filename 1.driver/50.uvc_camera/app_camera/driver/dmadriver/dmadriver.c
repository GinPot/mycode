//lsmod名字和ko名字一样


#include <linux/module.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include <linux/fs.h>
#include <linux/cdev.h>

#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>

#define DEV_MAJOR	0					//设置0，即用动态分配的
#define DEV_MINOR	0
#define DEV_NAME	"driver_test"		//该name好像没啥用


static dev_t devt;
static struct cdev *cdev;
static struct class *driverClass;
static struct device *driverDev;

struct DmaInfo{
	char *physrc;
	int sizey;
	char *phydesty;
	char *phydestc;
} dmainfo;

dma_cap_mask_t dma_test_mask;				//位掩码，设置dma通道的传输类型
struct dma_chan *chan = NULL;				//DMA主结构体
struct completion	rx_dma_complete;



static int DmaDriverOpen(struct inode *inode, struct file *file)
{
	pr_notice("%s: init dma engine.\n", __func__);
	
	dma_cap_zero(dma_test_mask);
	dma_cap_set(DMA_MEMCPY, dma_test_mask); 												//Memory to memory copy
	chan = dma_request_chan_by_mask(&dma_test_mask);
	if (IS_ERR(chan)) {
		pr_err("dma request channel failed\n");
		return -1;
	}
	init_completion(&rx_dma_complete);														//初始化dma完成flag

	return 0;
}

static void dma_test_callback(void *param)
{
	complete(&rx_dma_complete);
}

static ssize_t DmaDriverWrite(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
//	unsigned char *dma_dst = NULL;
	struct dma_async_tx_descriptor *tx;
	dma_cookie_t cookie;
	int ret = 0;
	
//	pr_notice("%s: enter\n", __func__);

	if (copy_from_user(&dmainfo, user_buf, count)) {
		pr_err("%s: failed to copy from user cfg_data.\n", __func__);
		return -EIO;
	}
	
	//pr_notice("physrc=0x%08lx, sizey=%d, phydesty=0x%08lx, phydestc=0x%08lx\n", (unsigned long)dmainfo.physrc, dmainfo.sizey, (unsigned long)dmainfo.phydesty, (unsigned long)dmainfo.phydestc);	
	
	
//	dma_dst =(unsigned char *) __phys_to_virt(dmainfo.physrc);
//	pr_notice("0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x\n",
//		dma_dst[0],dma_dst[1],dma_dst[2],dma_dst[3],dma_dst[4],dma_dst[5],dma_dst[6],dma_dst[7],dma_dst[8],dma_dst[9],
//		dma_dst[10],dma_dst[11],dma_dst[12],dma_dst[13],dma_dst[14],dma_dst[15],dma_dst[16],dma_dst[17],dma_dst[18],dma_dst[19],
//		dma_dst[20],dma_dst[21],dma_dst[22],dma_dst[23],dma_dst[24],dma_dst[25],dma_dst[26],dma_dst[27],dma_dst[28],dma_dst[29],
//		dma_dst[30],dma_dst[31],dma_dst[32],dma_dst[33],dma_dst[34],dma_dst[35],dma_dst[36],dma_dst[37],dma_dst[38],dma_dst[39],
//		dma_dst[40],dma_dst[41],dma_dst[42],dma_dst[43],dma_dst[44],dma_dst[45],dma_dst[46],dma_dst[47],dma_dst[48],dma_dst[49]);

	
	tx = dmaengine_prep_dma_memcpy(chan, (dma_addr_t)dmainfo.phydesty, (dma_addr_t)dmainfo.physrc, dmainfo.sizey, DMA_CTRL_ACK | DMA_PREP_INTERRUPT);
	if (!tx) {
		pr_err("device_prep_dma_memcpy error\n");
		ret = -EIO;
	}
	
	reinit_completion(&rx_dma_complete);												//设置dma完成flag为0
	tx->callback = dma_test_callback;													//设置dma完成后的回调函数
	//tx->callback_param = cqspi;
	cookie = tx->tx_submit(tx);															//开始dma传输
	ret = dma_submit_error(cookie);														//检测dma传输是否提交成功
	if (ret) {
		pr_err("dma_submit_error %d\n", cookie);
		ret = -EIO;
	}
	dma_async_issue_pending(chan);
	if(!wait_for_completion_timeout(&rx_dma_complete, msecs_to_jiffies(1000))) {		//睡眠等待dma中断的回调函数唤醒
		dmaengine_terminate_sync(chan);
		pr_err("DMA wait_for_completion_timeout\n");
		ret = -ETIMEDOUT;
	}

/*************************************************************************************************************************************/

	
	tx = dmaengine_prep_dma_memcpy(chan, (dma_addr_t)dmainfo.phydestc, (dma_addr_t)(dmainfo.physrc + dmainfo.sizey), dmainfo.sizey/2, DMA_CTRL_ACK | DMA_PREP_INTERRUPT);
	if (!tx) {
		pr_err("device_prep_dma_memcpy error\n");
		ret = -EIO;
	}
	
	reinit_completion(&rx_dma_complete);												//设置dma完成flag为0
	tx->callback = dma_test_callback;													//设置dma完成后的回调函数
	//tx->callback_param = cqspi;
	cookie = tx->tx_submit(tx);															//开始dma传输
	ret = dma_submit_error(cookie);														//检测dma传输是否提交成功
	if (ret) {
		pr_err("dma_submit_error %d\n", cookie);
		ret = -EIO;
	}
	dma_async_issue_pending(chan);
	if(!wait_for_completion_timeout(&rx_dma_complete, msecs_to_jiffies(100))) {			//睡眠等待dma中断的回调函数唤醒
		dmaengine_terminate_sync(chan);
		pr_err("DMA wait_for_completion_timeout\n");
		ret = -ETIMEDOUT;
	}	

	
//	dma_dst =(unsigned char *) __phys_to_virt(dmainfo.phydesty);
//	pr_notice("0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x\n",
//		dma_dst[0],dma_dst[1],dma_dst[2],dma_dst[3],dma_dst[4],dma_dst[5],dma_dst[6],dma_dst[7],dma_dst[8],dma_dst[9],
//		dma_dst[10],dma_dst[11],dma_dst[12],dma_dst[13],dma_dst[14],dma_dst[15],dma_dst[16],dma_dst[17],dma_dst[18],dma_dst[19],
//		dma_dst[20],dma_dst[21],dma_dst[22],dma_dst[23],dma_dst[24],dma_dst[25],dma_dst[26],dma_dst[27],dma_dst[28],dma_dst[29],
//		dma_dst[30],dma_dst[31],dma_dst[32],dma_dst[33],dma_dst[34],dma_dst[35],dma_dst[36],dma_dst[37],dma_dst[38],dma_dst[39],
//		dma_dst[40],dma_dst[41],dma_dst[42],dma_dst[43],dma_dst[44],dma_dst[45],dma_dst[46],dma_dst[47],dma_dst[48],dma_dst[49]);
	
	return ret;
}

static int DmaDriverRelease(struct inode *inode, struct file *file)
{
	pr_notice("%s: close\n", __func__);

	if(chan)
		dma_release_channel(chan);
	
	return 0;
}

static const struct file_operations fops = {
	.owner 			= THIS_MODULE,
	.open 			= DmaDriverOpen,
	.write 			= DmaDriverWrite,
	.release 		= DmaDriverRelease,
};




static int DmaDriverInit(void)
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

	driverClass = class_create(THIS_MODULE, "dmadriverClass");							//kset_register创建/sys/class/dmadriverClass/目录
	if(IS_ERR(driverClass)){															//创建class不是创建节点的重点，只是为下面函数提供参数
		pr_err("%s: fail to class create.\n", __func__);
		ret = PTR_ERR(driverClass);
		goto err_cdev;
	}

	driverDev = device_create(driverClass, NULL, devt, NULL, "%s","dmadriver");		//1、指定主从设备好devt创建device及/dev/driverTest节点
	if(IS_ERR(driverDev)){															//2、是否会创建dev节点的关键：是否会传入主次设备号devt;传入MKDEV(0, 0)即不会生成设备节点,不能为NULL
		pr_err("%s: fail to device create.\n", __func__);							//3、第二个参数NULL，即没有父设备；将会再/sys/devices/virtual/dmadriverClass/创建对应的设备
		ret = PTR_ERR(driverDev);
		goto err_device;
	}
	ret = dma_coerce_mask_and_coherent(driverDev, DMA_BIT_MASK(32));				//设置设备通过DMA方式可寻址的物理地址范围
	if (ret)
		goto err_dma;


	return ret;

err_dma:
	device_destroy(driverClass, devt);
err_device:
	class_destroy(driverClass);
err_cdev:
	cdev_del(cdev);
err_chrdev:
	unregister_chrdev_region(devt, 1);
	return ret;
}

static void DmaDriverExit(void)
{
	pr_notice("%s\n", __func__);

	device_destroy(driverClass, devt);
	class_destroy(driverClass);
	cdev_del(cdev);
	unregister_chrdev_region(devt, 1);												//第二个参数为次设备号个数
}

MODULE_LICENSE("GPL");
module_init(DmaDriverInit);
module_exit(DmaDriverExit);