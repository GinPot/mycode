#include <linux/module.h>
#include <linux/device.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>

#include <linux/dmapool.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>



struct MemData {
	struct device *dev;

	//struct resource *res;
	//void __iomem *base;
	
	//struct device_node *res_node;
	////struct reserved_mem *rmem;
	//struct resource rmem;
	//void *vaddr;

	dma_addr_t addr;
	void *vaddr;
	struct dma_pool *mem_pool;
};


static int mem_test_probe(struct platform_device *pdev)
{
	int rc = 0;

	struct MemData *memdata = NULL;
	dev_info(&pdev->dev, "%s %d: enter\n", __func__, __LINE__);

	memdata = devm_kzalloc(&pdev->dev, sizeof(*memdata), GFP_KERNEL);
	if(!memdata){
		dev_err(&pdev->dev, "%s %d: out of memory\n", __func__, __LINE__);
		return -ENOMEM;
	}
	memdata->dev = &pdev->dev;
	platform_set_drvdata(pdev, memdata);


	//例子0：ioremap
	//void __iomem *gic_data_dist_dist_target;
	//
	//gic_data_dist_dist_target = ioremap(0x01c81000, 0x1000);								//一般结合request_mem_region使用，现在多用下面例子1的方案
	//for (i = 32; i < 224; i += 4) {
	//	mask = readl_relaxed(gic_data_dist_dist_target + 0x800 + i);
	//	dev_info(&pdev->dev, "%s %d: i=%d mask=0x%x\n", __func__, __LINE__, i, mask);
	//}



	//例子1：寄存器地址访问
	//memdata->res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	//memdata->base = devm_ioremap_resource(&pdev->dev, memdata->res);						//处理申请(request_mem_region)与重映射操作, 相关函数devm_ioremap()
	//if (IS_ERR(memdata->base)) {
	//	dev_err(&pdev->dev, "Failed to map the registers\n");
	//	return PTR_ERR(memdata->base);
	//}
	//writel(BIT(0)|BIT(1), memdata->base+0x4);
	//dev_info(&pdev->dev, "%s %d: 0x%llx=0x%x\n", __func__, __LINE__, memdata->res->start+0x4, readl(memdata->base+0x4));



	//例子2： 设备数中预留no-map的内存，代码中获取内存、映射、访问
	//of_iomap()直接ioremap映射设备树节点的内存得到虚拟地主
	//memdata->res_node = of_parse_phandle(pdev->dev.of_node, "memory-region", 0);			//获取memory-region = <&memtest>对应的保留内存节点节点
	//if (!memdata->res_node){
	//	dev_err(&pdev->dev, "of_parse_phandle Failed\n");
	//	return -ENODEV;
	//}
	//	//memdata.rmem = __find_rmem(memdata->res_node);									//根据节点取出对应保留内存的信息,参考 of_reserved_mem_device_init()
	//if (of_address_to_resource(memdata->res_node, 0, &memdata->rmem)) {					//通过设备树节点获取保留内存的信息
	//	of_node_put(pdev->dev.of_node);
	//	dev_err(&pdev->dev, "of_address_to_resource Failed\n");
	//	return -EINVAL;
	//}
	//of_node_put(pdev->dev.of_node);
	//	//MEMREMAP_WB: 针对系统的DDR内存，直接返回__phys_to_virt已经映射好的虚拟地址
	//	//MEMREMAP_WT: 创建映射，__ioremap((addr), (size), __pgprot(PROT_DEVICE_nGnRE))
	//	//MEMREMAP_WC: 创建映射，__ioremap((addr), (size), __pgprot(PROT_NORMAL_NC))
	//memdata->vaddr = devm_memremap(&pdev->dev, memdata->rmem.start, resource_size(&memdata->rmem), MEMREMAP_WB);		//指定物理，映射虚拟地址，返回虚拟地址；实际上是调用memremap()建立虚拟地址映射
	//if (IS_ERR(memdata->vaddr)) {
	//	dev_err(&pdev->dev, "Failed to memremap\n");
	//	return PTR_ERR(memdata->vaddr);
	//}
	//writel(0x88888, memdata->vaddr);														//可以直接操作地址、memset、memcpy等
	//dev_info(&pdev->dev, "%s %d: 0x%llx=0x%x\n", __func__, __LINE__, memdata->rmem.start, readl(memdata->vaddr));



	//例子3:DMA
	//3.1.1、获取设备树指定该设备的保留内存用作dma
	//rc = of_reserved_mem_device_init(&pdev->dev);											//获取设备树种指定的保留内存节点信息，映射，把相关信息保存到dev->dma_mem中
	//if(rc) {
	//	dev_err(&pdev->dev, "Could not get reserved memory\n");
	//	return rc;
	//}
	//3.1.2、代码中直接指定一段内存给该设备用作dma
	//rc = dma_declare_coherent_memory(&pdev->dev, 0x70000000, 0x70000000, 0x100000);		//第二第三个参数一样都是物理地址,第四个参数是内存大小，作用类似于of_reserved_mem_device_init()
	//if (rc < 0) {
	//	dev_err(&pdev->dev, "Failed to associate buffer\n");
	//	return rc;
	//}
	
	//3.1.3、设置了全局cma区域的情况下，没有3.1.1和3.1.1时，将直接从cma区申请内存

	rc = dma_coerce_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));						//设置设备通过DMA方式可寻址的物理地址范围
	if (rc < 0){
		dev_err(&pdev->dev, "error: failed to set DMA mask %d\n", rc);
		goto err;
	}
	
	//3.2.1、最小申请内存为一页(4K)，最大内存收cma池大小限制,常用于分配较大DMA buffer
	//memdata->vaddr = dma_alloc_coherent(&pdev->dev, 200*1024*1024, &memdata->addr, GFP_KERNEL);	//最小申请内存为一页(4K),分配较大DMA buffer，直接申请nocache的内存,因为前面指定了dev->dma_mem,将从memtest节点保留内存中申请dma内存
	//if(!memdata->vaddr){
	//	dev_err(&pdev->dev, "%s: src failed to allocate coherent memory for hardware descriptors.\n", __func__);
	//	rc = -ENOMEM;
	//	goto err;
	//}

	//3.2.2、最小申请内存小于4K的情况下用内存池的方式
	//第三个参数： 从缓存池中分配的大小，即用dma_pool_alloc分配大小，不是指整个池的大小；
	//第四个参数： 第三个参数的对齐大小；
	//第五个参数： 内存边界限制；设置为0的话默认取第三个参数和页框中最大的值；如设置为4096，表示从内存池(第三个参数)分配的内存不能超过4096
	//一般用于申请小于4K的情况下，
	//dma_pool_alloc第一次先通过dma_alloc_coherent申请第三个参数和页框中最大的值的内存，如4K； 在从中4k中分配第三个参数大小的内存给驱动使用；第二次就优先从池中申请内存
	memdata->mem_pool = dma_pool_create("MEMORY_POOL", &pdev->dev, 1*1024, 4, 0);
	if (!memdata->mem_pool) {
		dev_err(&pdev->dev, "unable to allocate descriptor pool\n");
		rc = -ENOMEM;
		goto err;
	}
	memdata->vaddr = dma_pool_alloc(memdata->mem_pool, GFP_ATOMIC, &memdata->addr);
	if (!memdata->vaddr) {
		dev_err(&pdev->dev, "can't allocate dma_pool_alloc 1*1024\n");
		rc = -ENOMEM;
		goto err;
	}

	writel(0x2222, memdata->vaddr);															//可以直接操作地址、memset、memcpy等
	dev_info(&pdev->dev, "%s %d: 0x%llx=0x%x\n", __func__, __LINE__, memdata->addr, readl(memdata->vaddr));

	dma_pool_free(memdata->mem_pool, memdata->vaddr, memdata->addr);

	return rc;
err:
	//of_reserved_mem_device_release(&pdev->dev);
	//dma_release_declared_memory(&pdev->dev);
	return rc;
}


static int mem_test_remove(struct platform_device *pdev)
{
	struct MemData *memdata = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "%s %d: enter\n", __func__, __LINE__);

	dma_pool_destroy(memdata->mem_pool);
	//dma_free_coherent(&pdev->dev, 200*1024*1024, memdata->vaddr, memdata->addr);
	of_reserved_mem_device_release(&pdev->dev);
	//dma_release_declared_memory(&pdev->dev);



	return 0;
}

static const struct of_device_id mem_dt_match[] = {
	{ .compatible = "mem-testfs", },
	{ },
};

static struct platform_driver mem_test_driver = {
	.driver = {
		.name			= "mem-testfs",
		.of_match_table	= mem_dt_match,
	},
	.probe	= mem_test_probe,
	.remove	= mem_test_remove,
};

module_platform_driver(mem_test_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Allwinner H5 mem test");
MODULE_AUTHOR("GinPot <GinPot@xxx.com>");


/*
对于32位的内存控制器来说，访问地址0x00000000，0x00000001，0x00000002，0x00000003，都是访问地址0x00000000
内存地址每+1，对应的就是增加1个字节大小空间，32位，即4个字节，所以一般访问地址是
0x00000000
0x00000004
0x00000008
...

内存、寄存器操作函数
#define readb(addr)		({ u8  __v = readb_relaxed(c); __iormb(__v); __v; })
#define readw(addr)		({ u16 __v = readw_relaxed(c); __iormb(__v); __v; })
#define readl(addr)		({ u32 __v = readl_relaxed(c); __iormb(__v); __v; })
#define readq(addr)		({ u64 __v = readq_relaxed(c); __iormb(__v); __v; })

#define writeb(value,addr)		({ __iowmb(); writeb_relaxed((v),(c)); })
#define writew(value,addr)		({ __iowmb(); writew_relaxed((v),(c)); })
#define writel(value,addr)		({ __iowmb(); writel_relaxed((v),(c)); })
#define writeq(value,addr)		({ __iowmb(); writeq_relaxed((v),(c)); })
*/


/*
例子1：寄存器地址访问
	soc{
		mem_test: mem-test@1c69000 {
			compatible = "mem-testfs";
			reg = <0x1c69000 0x100>;

			status = "okay";
		};
	};



//例子2： 设备数中预留no-map的内存，代码中获取内存、映射、访问
//例子3.1.1： dma方式申请内存;驱动绑定制定的保留内存
	reserved-memory {
		#address-cells = <1>;
		#size-cells = <1>;
		ranges;
	
		memtest: linux,memtest@70000000 {
			compatible = "shared-dma-pool";
			no-map;											//不会给到伙伴系统，内核不会直接映射[    0.000000] Reserved memory: created DMA memory pool at 0x0000000070000000, size 16 MiB
			reg = <0x70000000 0x1000000>;
		};
	};
	mem_test{
		compatible = "mem-testfs";
		memory-region = <&memtest>;
		status = "okay";
	};

//例子3.1.3： dma方式申请内存;设置全局的cma保留区域；
在调用dma_alloc_coherent()申请内存时，
	1、先确认dev->dma_mem是否有指定的dma内存
	2、没有的话就从全局的cma区域申请
	reserved-memory {
		#address-cells = <1>;
		#size-cells = <1>;
		ranges;

		linux,cma {
			compatible = "shared-dma-pool";
			reusable;										//操作系统的其他用途也可以用这块内存，但当dma需要时，其他的内存需要迁移出去
			reg = <0x60000000 0x10000000>;
			linux,cma-default;								//linux将使用该区域作为连续内存分配器的默认池
		};
	};


DMA设备提供了两类pool，一个device pool，一个global pool，两个pool的内存在分配时都会被映射成non-cache的
Device DMA Pool：	
	1、参考例子3.1.1； Device Pool的Memory在DTS中reserve，在reserve-memory中预留，并在节点中通过配置compatible:”shared-dma-pool”+”no-map”来声明这块reserve memory是为device预留的dma buffer pool memory，在device初始化时，dts中通过memory-region引入这类reserve-memory，会将这块物理内存绑定到device->dma_mem上
	2、参考例子3.1.2； 可以通过dma_declare_coherent_memory为一块物理内存建立non-cache映射，并直接绑定到device->dma_mem上
Global DMA Pool
	参考例子3.1.3； 
	dts中命名为”linux,dma-default”的reserve memory会被识别成默认global dma coherent buffer pool，映射后的地址会保存在dma_coherent_default_memory全局变量中
	在通过dma_declare_coherent申请dma buffer时，如果device没有绑定device dma mem，则会从这个global coherent pool申请
*/


/*

#define dma_map_single(d, a, s, r) dma_map_single_attrs(d, a, s, r, 0)
static inline dma_addr_t dma_map_single_attrs(struct device *dev, void *ptr, size_t size, enum dma_data_direction dir, unsigned long attrs)

kmalloc申请的连续物理地址的虚拟地址，建立流映射，范围DMA用的物理地址，并根据dir设置了对应的缓存
一般流映射后cpu不能访问这段内存，但可以在下面两个函数中间访问：
static inline void dma_sync_single_for_cpu(struct device *dev, dma_addr_t addr, size_t size, enum dma_data_direction dir)
static inline void dma_sync_single_for_device(struct device *dev, dma_addr_t addr, size_t size, enum dma_data_direction dir)					   

#define dma_unmap_single(d, a, s, r) dma_unmap_single_attrs(d, a, s, r, 0)
static inline void dma_unmap_single_attrs(struct device *dev, dma_addr_t addr, size_t size, enum dma_data_direction dir, unsigned long attrs)




单页流映射，就是只映射一页缓存

dma_addr_t dma_map_page(struct device *dev, struct page *page,
size_t offset, size_t size,enum dma_data_direction dir)
第一个参数：dev设备指针
第二个参数：页指针，用__get_free_page分配
第三个参数：页偏移量
第四个参数：只映射一页的部分，字节单位，size小于PAGE_SIZE
第五个参数：流方向标记
返回值：返回总线地址（物理地址）

void dma_unmap_page(struct device *dev, dma_addr_t addr,
size_t size, enum dma_data_direction dir)
第一个参数：dev设备指针
第二个参数：addr总线地址（物理地址），由dma_map_page返回的
第三个参数：缓存大小，同dma_map_page第四个参数
第四个参数：流方向标志。


发散/汇聚映射： https://blog.csdn.net/eZiMu/article/details/54959269
*/

