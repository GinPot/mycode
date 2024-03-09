
/*
kmalloc ：分配内核内存，保证分配的内存在物理上是连续的，与真实地址只有一个偏差，最大4M，可以用于DMA访问(需要no cache下)，不进行初始化，内存不足时也会进入税前。         				kfree()释放;

vmalloc ：分配虚拟内核内存，保证在虚拟地址上是连续的(在物理地址上可能不连续，需要一个个映射)，比kmalloc慢，可能睡眠,申请大小没有限制						   			vfree()释放;

kzalloc ：kmalloc + memset	,即分配时会进行初始化

devm_kzalloc：具有资源管理的kzalloc，使用资源管理函数分配内存，与所属设备相关联，设备从系统中分离，内存会被自动释放，也可手动释放

malloc：分配用户内存，free释放

*/




#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/device.h>

#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>

static struct class *driverClass;
static struct device *driverDev;

dma_cap_mask_t dma_test_mask;				//位掩码，设置dma通道的传输类型
struct dma_chan *chan = NULL;				//DMA主结构体

char *dst, *src, *src1;
dma_addr_t dma_dst, dma_src;
struct completion	rx_dma_complete;
#define DMA_DATA_LEN		4*1024*1024



static void dma_test_callback(void *param)
{
	complete(&rx_dma_complete);
}

static int dma_test_init(void)
{
	int ret=0;
	dma_cookie_t cookie;
	struct dma_async_tx_descriptor *tx;
	
    pr_notice("%s: ginpot enter\n", __func__);

	/**************char******************/
	
	driverClass = class_create(THIS_MODULE, "DmaTestClass");								//kset_register创建/sys/class/driverClass/目录
	if(IS_ERR(driverClass)){																//创建class不是创建节点的重点，只是为下面函数提供参数
		pr_err("%s: fail to class create.\n", __func__);
		ret = PTR_ERR(driverClass);
		return ret;
	}

	driverDev = device_create(driverClass, NULL, MKDEV(0, 0), NULL, "%s","dmatest");		//1、指定主从设备号devt创建device及/dev/driverTest节点
	if(IS_ERR(driverDev)){																	//2、是否会创建dev节点的关键：是否会传入主次设备号devt,传入MKDEV(0, 0)即不会生成设备节点,不能为NULL
		pr_err("%s: fail to device create.\n", __func__);									//3、第二个参数NULL，即没有父设备；将会再/sys/devices/virtual/driverClass/创建对应的设备
		ret = PTR_ERR(driverDev);
		goto err_device;
	}

	ret = dma_coerce_mask_and_coherent(driverDev, DMA_BIT_MASK(32));						//设置设备通过DMA方式可寻址的物理地址范围
	if (ret)
		goto err_src;

	/**************mem******************/
	
	src = kzalloc(DMA_DATA_LEN, GFP_KERNEL);
	if(!src){
		pr_err("%s: src no %d memory.\n", __func__, DMA_DATA_LEN);
		return -ENOMEM;
		goto err_src;
	}
	memset(src, 0x88, DMA_DATA_LEN);
	
	dst = kzalloc(DMA_DATA_LEN, GFP_KERNEL);
	if(!dst){
		pr_err("%s: dst no %d memory.\n", __func__, DMA_DATA_LEN);
		ret =  -ENOMEM;
		goto err_dst;
	}
	
	pr_info("src: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x.\n", src[0], src[1], src[2], src[3], src[4], src[DMA_DATA_LEN-5], src[DMA_DATA_LEN-4], src[DMA_DATA_LEN-3], src[DMA_DATA_LEN-2], src[DMA_DATA_LEN-1]);
	pr_info("dst: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x.\n", dst[0], dst[1], dst[2], dst[3], dst[4], dst[DMA_DATA_LEN-5], dst[DMA_DATA_LEN-4], dst[DMA_DATA_LEN-3], dst[DMA_DATA_LEN-2], dst[DMA_DATA_LEN-1]);


	dma_dst = dma_map_single(driverDev, dst, DMA_DATA_LEN, DMA_BIDIRECTIONAL);				//把内存no cache操作
	if (dma_mapping_error(driverDev, dma_dst)) {
		pr_err("dma_dst mapping failed\n");
		goto err_dma_dst;
	}

	dma_src = dma_map_single(driverDev, src, DMA_DATA_LEN, DMA_BIDIRECTIONAL);
	if (dma_mapping_error(driverDev, dma_src)) {
		pr_err("dma_src mapping failed\n");
		goto err_dma_src;
	}
	pr_info("dma_dst=0x%llx, dma_src=0x%llx\n", dma_dst, dma_src);
	
	
	src1 = (char *)__phys_to_virt(dma_src);
	pr_info("dst: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x.\n", src1[0], src1[1], src1[2], src1[3], src1[4], src1[DMA_DATA_LEN-5], src1[DMA_DATA_LEN-4], src1[DMA_DATA_LEN-3], src1[DMA_DATA_LEN-2], src1[DMA_DATA_LEN-1]);

	
	/**************dma******************/

	dma_cap_zero(dma_test_mask);
	dma_cap_set(DMA_MEMCPY, dma_test_mask); 												//Memory to memory copy
	chan = dma_request_chan_by_mask(&dma_test_mask);
	if (IS_ERR(chan)) {
		pr_err("dma request channel failed\n");
		goto err_dma;
	}
	init_completion(&rx_dma_complete);														//初始化dma完成flag

	
	
	tx = dmaengine_prep_dma_memcpy(chan, dma_dst, dma_src, DMA_DATA_LEN, DMA_CTRL_ACK | DMA_PREP_INTERRUPT);
	if (!tx) {
		pr_err("device_prep_dma_memcpy error\n");
		ret = -EIO;
		goto err_prep_dma;
	}
	
	reinit_completion(&rx_dma_complete);												//设置dma完成flag为0
	tx->callback = dma_test_callback;													//设置dma完成后的回调函数
	//tx->callback_param = cqspi;
	cookie = tx->tx_submit(tx);															//开始dma传输
	ret = dma_submit_error(cookie);														//检测dma传输是否提交成功
	if (ret) {
		pr_err("dma_submit_error %d\n", cookie);
		ret = -EIO;
		goto err_prep_dma;
	}
	dma_async_issue_pending(chan);
	if(!wait_for_completion_timeout(&rx_dma_complete, msecs_to_jiffies(100))) {			//睡眠等待dma中断的回调函数唤醒
		dmaengine_terminate_sync(chan);
		pr_err("DMA wait_for_completion_timeout\n");
		ret = -ETIMEDOUT;
		goto err_prep_dma;
	}
	
	pr_info("src: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x.\n", src[0], src[1], src[2], src[3], src[4], src[DMA_DATA_LEN-5], src[DMA_DATA_LEN-4], src[DMA_DATA_LEN-3], src[DMA_DATA_LEN-2], src[DMA_DATA_LEN-1]);
	pr_info("dst: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x.\n", dst[0], dst[1], dst[2], dst[3], dst[4], dst[DMA_DATA_LEN-5], dst[DMA_DATA_LEN-4], dst[DMA_DATA_LEN-3], dst[DMA_DATA_LEN-2], dst[DMA_DATA_LEN-1]);


	
	return 0;

	

err_prep_dma:
	if(chan)
		dma_release_channel(chan);
err_dma:	
	if(dma_src)
		dma_unmap_single(driverDev, dma_src, DMA_DATA_LEN, DMA_BIDIRECTIONAL);
err_dma_src:	
	if(dma_dst)
		dma_unmap_single(driverDev, dma_dst, DMA_DATA_LEN, DMA_BIDIRECTIONAL);
err_dma_dst:
	if(src)
		kfree(src);
err_dst:
	if(dst)
		kfree(dst);
err_src:
	device_destroy(driverClass, MKDEV(0, 0));
err_device:
	class_destroy(driverClass);


    return ret;
}

static void dma_test_exit(void)
{
    pr_notice("%s: enter\n", __func__);


	
	if(chan)
		dma_release_channel(chan);
	
	if(dma_dst)
		dma_unmap_single(driverDev, dma_dst, DMA_DATA_LEN, DMA_BIDIRECTIONAL);
	if(dma_src)
		dma_unmap_single(driverDev, dma_src, DMA_DATA_LEN, DMA_BIDIRECTIONAL);

	if(src)
		kfree(src);
	if(dst)
		kfree(dst);

	device_destroy(driverClass, MKDEV(0, 0));
	class_destroy(driverClass);

}

MODULE_LICENSE("GPL");
module_init(dma_test_init);
module_exit(dma_test_exit);


