#include <linux/delay.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>



struct irqDrv {
	struct device *dev;
	int gpio;
	int irq;
};

//struct irq_var {
//	int gpio;
//	int num;
//};



//在使能线程中断时，若没有IRQF_ONESHOT，中断线程仍是需要退出后才能再次被触发调用； 
//但中断上半部因退出的比较快，又使能中断，在电平触发时又会频繁的触发调用中断上半部；
//所以一般在电平触发(或中断频繁触发的情况)+线程中断化的情况下需要设置IRQF_ONESHOT等待中断线程退出后再使能中断
//创建的线程名：irq/46-irq_test(irq/硬件中断号-软件name)
static irqreturn_t irq_test_threaded(int irq, void *data)					//中断线程可以加sleep
{
	struct irqDrv *irqdrv = (struct irqDrv *)data;

	dev_info(irqdrv->dev, "%s %d: irq=%d\n", __func__, __LINE__, irq);

	msleep(100);


	return IRQ_HANDLED;
};

//devm_request_threaded_irq()注册中断的时候创建的这个内核线程
static irqreturn_t irq_test_handler(int irq, void *data)					//在中断上半部加sleep调用schedule直接crash
{
//	struct irqDrv *irqdrv = (struct irqDrv *)data;

//	dev_info(irqdrv->dev, "%s %d: irq=%d\n", __func__, __LINE__, irq);

	//dump_stack();

	return IRQ_WAKE_THREAD;
}

static int irq_test_probe(struct platform_device *pdev)
{
//	const struct irq_var *irqdata;
	int ret;
	struct irqDrv *irqdrv;

	int parent_irq;
	struct of_phandle_args out_irq;

	void __iomem *gic_data_dist_dist_target;
	u32 mask, i;

/*********************************************************/
	uint64_t __val;
	asm volatile("mrs %0, DAIF "	
	: "=r" (__val));

	dev_info(&pdev->dev, "%s %d: enter, DAIF=0x%llx\n", __func__, __LINE__, __val);
/*********************************************************/

	gic_data_dist_dist_target = ioremap(0x01c81000, 0x1000);
	for (i = 32; i < 224; i += 4) {
		mask = readl_relaxed(gic_data_dist_dist_target + 0x800 + i);
		dev_info(&pdev->dev, "%s %d: i=%d mask=0x%x\n", __func__, __LINE__, i, mask);
	}

/*********************************************************/
//	irqdata = of_device_get_match_data(&pdev->dev);
//	if (!irqdata){
//		dev_err(&pdev->dev, "%s: get platform data fail\n", __func__);
//		return -EINVAL;
//	}
//	dev_info(&pdev->dev, "%s: irqdata->gpio=%d irqdata->num=%d\n", __func__, irqdata->gpio, irqdata->num);	

	irqdrv = devm_kzalloc(&pdev->dev, sizeof(struct irqDrv), GFP_KERNEL);
	if(!irqdrv){
		dev_err(&pdev->dev, "%s %d: malloc irqdrv mem fail\n", __func__, __LINE__);
		return -ENOMEM;
	}
	
	irqdrv->dev = &pdev->dev;
	platform_set_drvdata(pdev, irqdrv);

	//irqdrv->gpio = of_get_gpio(pdev->dev.of_node, 0);
//	irqdrv->gpio = of_get_named_gpio(pdev->dev.of_node, "irq-gpio", 0);
//	if (!gpio_is_valid(irqdrv->gpio)) {
//		dev_err(&pdev->dev, "%s %d: gpio is invalid\n", __func__, __LINE__);
//		return irqdrv->gpio; // return -ENOENT;
//	}	
//	dev_info(&pdev->dev, "%s %d: irqdrv->gpio=%d\n", __func__, __LINE__, irqdrv->gpio);
//	
//	ret = devm_gpio_request_one(&pdev->dev, irqdrv->gpio, GPIOF_IN, pdev->name);
//	if (ret){
//		dev_err(&pdev->dev, "failed to request gpio\n");
//		return ret;
//	}
//	irqdrv->irq = gpio_to_irq(irqdrv->gpio);
//	if(irqdrv->irq < 0){
//		dev_err(&pdev->dev, "gpio to irq fail\n");
//		return irqdrv->irq;
//	}

	//interrupts = <GIC_SPI 14 IRQ_TYPE_LEVEL_HIGH>;
//	irqdrv->irq = platform_get_irq(pdev, 1);
//	if (irqdrv->irq < 0) {
//		dev_err(&pdev->dev, "Can't get platform interrupt\n");
//		return irqdrv->irq;
//	}
//	ret = devm_request_threaded_irq(&pdev->dev, irqdrv->irq, irq_test_handler, irq_test_threaded, 0 | IRQF_ONESHOT, pdev->name, irqdrv);
//	dev_info(&pdev->dev, "%s %d spi: irqdrv->irq=%d\n", __func__, __LINE__, irqdrv->irq);

	irqdrv->irq = platform_get_irq(pdev, 0);
	if (irqdrv->irq < 0) {
		dev_err(&pdev->dev, "Can't get platform interrupt\n");
		return irqdrv->irq;
	}
	dev_info(&pdev->dev, "%s %d gpio: irqdrv->irq=%d\n", __func__, __LINE__, irqdrv->irq);
	//ret = devm_request_irq(&pdev->dev, irqdrv->irq, irq_test_handler, IRQF_TRIGGER_FALLING, pdev->name, irqdrv);		//只创建中断handler顶半部（top half）,没有创建中断处理线程
	ret = devm_request_threaded_irq(&pdev->dev, irqdrv->irq, irq_test_handler, irq_test_threaded, IRQF_TRIGGER_FALLING | IRQF_ONESHOT, pdev->name, irqdrv);
	if (ret) {
		dev_err(&pdev->dev, "request irq %d failed\n", irqdrv->irq);
		return ret;
	}


	/*
		/proc/irq/linux中断号/effective_affinity									//根据设置的smp_affinity值，即便设置亲和多核，会优化为CPU和数字偏小的那个一个核处理，不会亲和多核；对应的bit[核心数]
		/proc/irq/linux中断号/effective_affinity_list							// 888 实际当前中断具体再哪个CPU上处理,对应0、1、2、3 
		/proc/irq/linux中断号/smp_affinity										//可以动态设置在哪个CPU上相应中断
		/proc/irq/linux中断号/smp_affinity										//对应smp_affinity值所占的bit位，没有实际用处
		irq_set_affinity()中断亲和性设置实际去设置中断控制寄存器GICD_ITARGETSRn（中断目标寄存器，Interrupt Targets Register）允许把硬件中断n转发到哪些处理器；
		硬件中断号必须是root中断控制上的(SPI)
	*/
	//ret = irq_set_affinity(irqdrv->irq, cpumask_of(3));
	//ret = irq_set_affinity_hint(irqdrv->irq, get_cpu_mask(1));
	//if (ret) {
	//	dev_err(&pdev->dev, "Failed to set cpu affinity for ethernet irq:%d\n", irqdrv->irq);
	//} else {
	//	dev_info(&pdev->dev, "CPU affinity hint set for ethernet irq:%d\n", irqdrv->irq);
	//}

	//设置中断控制器亲和性;
	//实际先通过驱动的节点找到中断控制器节点，再获取中断控制器的软件中断号，再设置这个软件中断号的亲和性
	of_parse_phandle_with_args(pdev->dev.of_node, "interrupts-extended", "#interrupt-cells", 0, &out_irq);	//0表示interrupts-extended的第一个cells
	parent_irq = of_irq_get(out_irq.np, 0);
	ret = irq_set_affinity_hint(parent_irq, get_cpu_mask(3));
	if (ret) {
		dev_err(&pdev->dev, "Failed to set cpu affinity for ethernet irq:%d\n", parent_irq);
	} else {
		dev_info(&pdev->dev, "CPU affinity hint set for ethernet irq:%d\n", parent_irq);
	}

	dev_info(&pdev->dev, "%s %d: probe success.\n", __func__, __LINE__);
	return 0;
}

static int irq_test_remove(struct platform_device *pdev)
{
//	struct irqDrv *irqdrv = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "%s %d: enter\n", __func__, __LINE__);


	return 0;
}

/******************************************************************/

//static const struct platform_device_id irq_id[] = {
//	{
//		.name = "irq_test",
//		.driver_data = AR5416_AR9100_DEVID,
//	},
//	{},
//};
//MODULE_DEVICE_TABLE(platform, irq_id);

//static const struct irq_var irq_data = {
//	.gpio	= 100,
//	.num	= 20,
//};
static const struct of_device_id irq_dt_match[] = {
	{
		.compatible = "irq-test",
//		.data = &irq_data,
	},
	{ },
};
MODULE_DEVICE_TABLE(of, irq_dt_match);

static struct platform_driver irq_test_driver = {
	.driver = {
		.name			= "irq",						//1. driver_override节点name和device_driver的name匹配; platform device通过设置/sys/devices/platform/soc/xxx/driver_override(其他platform device设备路径可能不一样)节点name
						//"1c2ac00.i2c"					//4. device_driver的name和 platform_device.name(寄存器.节点名或者就是节点名1000000.clock or irq_test)匹配
		.of_match_table	= irq_dt_match,					//2. 用of_match_table里的name和备树的compatible匹配
	},
	//.id_table = irq_id,								//3. 用id_table的name和 platform_device.name 匹配
	//.probe_type = PROBE_PREFER_ASYNCHRONOUS			//设置该驱动可以异步加载
	.probe	= irq_test_probe,
	.remove	= irq_test_remove,
};
module_platform_driver(irq_test_driver);

MODULE_LICENSE("GPL");




/*
dts:
	irq_test_pins: irq-test-pins {
		pins = "PL3";
		function = "gpio_in";
		drive-strength = <16>;
		bias-pull-down;
	}

	irq_test {
		compatible = "irq-test";

		pinctrl-names = "default";									//配置复用功能时的用法，在驱动probe之前配置好GPIO相关功能
		pinctrl-0 = <&irq_test_pins>;

		interrupts-extended = <&r_pio 0 3 GPIO_ACTIVE_HIGH>,		//最新获取中断方法platform_get_irq(pdev, index);第二个参数为这个属性的第几个中断
							  <&gic GIC_SPI 14 IRQ_TYPE_LEVEL_HIGH>;//&gic指中断控制器的节点，其后面的参数个数根据中断控制器节点#interrupt-cells决定

		//先获取GPIO，再gpio_to_irq得到中断号
		//gpios = <&r_pio 0 3 GPIO_ACTIVE_HIGH>; 					// GPIOL3  of_get_gpio(pdev->dev.of_node, 0);第二个参数为dts中<>里面(可以写多个GPIO)的index
		//irq-gpio = <&r_pio 0 3 GPIO_ACTIVE_HIGH>;					// GPIOL3  of_get_named_gpio(pdev->dev.of_node, "irq-gpio", 0);第三个参数同上

		//interrupt-parent = <&r_pio>;								//同interrupts-extended
		//interrupts = <0 3 GPIO_ACTIVE_HIGH>;

		//interrupts = <GIC_SPI 14 IRQ_TYPE_LEVEL_HIGH>;			//直接获取中断号，一般soc驱动使用
		//interrupt-names = "mc";	

		//tp-irq-gpio = <143>;										//另类的方法获取GPIO号,少用;of_property_read_u32(dev->of_node, "tp-irq-gpio", &irq_gpio);
	};


中断返回状态：
	IRQ_NONE		= (0 << 0),								//中断不是该handler处理的，请找其他handler处理
	IRQ_HANDLED		= (1 << 0),								//中断已有handler处理完成
	IRQ_WAKE_THREAD	= (1 << 1),								//唤醒中断线程处理中断下半部

中断触发类型：
	#define IRQF_TRIGGER_RISING		0x00000001				//变化的时候触发
	#define IRQF_TRIGGER_FALLING	0x00000002				//变化的时候触发
	#define IRQF_TRIGGER_HIGH		0x00000004				//高电平循环触发，即退出hardler使能中断后又立即触发
	#define IRQF_TRIGGER_LOW		0x00000008				//低电平循环触发，即退出hardler使能中断后又立即触发

中断标注位：
	IRQF_DISABLED	
			首先要说明的是这是一个废弃的flag，在新的内核中，该flag没有任何的作用了。具体可以参考：Disabling IRQF_DISABLED
			旧的内核（2.6.35版本之前）认为有两种interrupt handler：slow handler和fast handle。在request irq的时候，对于fast handler，需要传递IRQF_DISABLED的参数，确保其中断处理过程中是关闭CPU的中断，
			因为是fast handler，执行很快，即便是关闭CPU中断不会影响系统的性能。但是，并不是每一种外设中断的handler都是那么快（例如磁盘），因此就有 slow handler的概念，说明其在中断处理过程中会耗时比较长。
			对于这种情况，在执行interrupt handler的时候不能关闭CPU中断，否则对系统的performance会有影响。
			新的内核已经不区分slow handler和fast handle，都是fast handler，都是需要关闭CPU中断的，那些需要后续处理的内容推到threaded interrupt handler中去执行。
	IRQF_SHARED	
			这是flag用来描述一个interrupt line是否允许在多个设备中共享。如果中断控制器可以支持足够多的interrupt source，那么在两个外设间共享一个interrupt request line是不推荐的，
			毕竟有一些额外的开销（发生中断的时候要逐个询问是不是你的中断，软件上就是遍历action list），因此外设的irq handler中最好是一开始就启动判断，看看是否是自己的中断，如果不是，返回IRQ_NONE,表示这个中断不归我管。 
			早期PC时代，使用8259中断控制器，级联的8259最多支持15个外部中断，但是PC外设那么多，因此需要irq share。现在，ARM平台上的系统设计很少会采用外设共享IRQ方式，毕竟一般ARM SOC提供的有中断功能的GPIO非常的多，足够用的。 
			当然，如果确实需要两个外设共享IRQ，那也只能如此设计了。对于HW，中断控制器的一个interrupt source的引脚要接到两个外设的interrupt request line上，怎么接？直接连接可以吗？当然不行，对于低电平触发的情况，我们可以考虑用与门连接中断控制器和外设。

	IRQF_PROBE_SHARED	
			IRQF_SHARED用来表示该interrupt action descriptor是允许和其他device共享一个interrupt line（IRQ number），但是实际上是否能够share还是需要其他条件：例如触发方式必须相同。有些驱动程序可能有这样的调用场景：
			我只是想scan一个irq table，看看哪一个是OK的，这时候，如果即便是不能和其他的驱动程序share这个interrupt line，我也没有关系，我就是想scan看看情况。这时候，caller其实可以预见sharing mismatche的发生，
			因此，不需要内核打印“Flags mismatch irq……“这样冗余的信息
	IRQF_PERCPU	
			在SMP的架构下，中断有两种mode，一种中断是在所有processor之间共享的，也就是global的，一旦中断产生，interrupt controller可以把这个中断送达多个处理器。当然，在具体实现的时候不会同时将中断送达多个CPU，
			一般是软件和硬件协同处理，将中断送达一个CPU处理。但是一段时间内产生的中断可以平均（或者按照既定的策略）分配到一组CPU上。这种interrupt mode下，interrupt controller针对该中断的operational register是global的，
			所有的CPU看到的都是一套寄存器，一旦一个CPU ack了该中断，那么其他的CPU看到的该interupt source的状态也是已经ack的状态。
			和global对应的就是per cpu interrupt了，对于这种interrupt，不是processor之间共享的，而是特定属于一个CPU的。例如GIC中interrupt ID等于30的中断就是per cpu的（这个中断event被用于各个CPU的local timer），这
			个中断号虽然只有一个，但是，实际上控制该interrupt ID的寄存器有n组（如果系统中有n个processor），每个CPU看到的是不同的控制寄存器。在具体实现中，这些寄存器组有两种形态，一种是banked，
			所有CPU操作同样的寄存器地址，硬件系统会根据访问的cpu定向到不同的寄存器，另外一种是non banked，也就是说，对于该interrupt source，每个cpu都有自己独特的访问地址。
	IRQF_NOBALANCING	
			这也是和multi-processor相关的一个flag。对于那些可以在多个CPU之间共享的中断，具体送达哪一个processor是有策略的，我们可以在多个CPU之间进行平衡。如果你不想让你的中断参与到irq balancing的过程中那么就设定这个flag
	IRQF_IRQPOLL	 
	IRQF_ONESHOT	
			one shot本身的意思的只有一次的，结合到中断这个场景，则表示中断是一次性触发的，不能嵌套。对于primary handler，当然是不会嵌套，但是对于threaded interrupt handler，
			我们有两种选择，一种是mask该interrupt source，另外一种是unmask该interrupt source。一旦mask住该interrupt source，那么该interrupt source的中断在整个threaded interrupt handler处理过程中都是不会再次触发的，
			也就是one shot了。这种handler不需要考虑重入问题。
			具体是否要设定one shot的flag是和硬件系统有关的，我们举一个例子，比如电池驱动，电池里面有一个电量计，是使用HDQ协议进行通信的，电池驱动会注册一个threaded interrupt handler，
			在这个handler中，会通过HDQ协议和电量计进行通信。对于这个handler，通过HDQ进行通信是需要一个完整的HDQ交互过程，如果中间被打断，整个通信过程会出问题，因此，这个handler就必须是one shot的。
			
			一般用在中断线程化时，且电平触发中断；线程化时表明下半部比较耗时，因为上半部可能会很快结束并使能中断导致中断又触发，所以加上这个标志等下半部处理完后再使能中断
			
	IRQF_NO_SUSPEND	
			这个flag比较好理解，就是说在系统suspend的时候，不用disable这个中断，如果disable，可能会导致系统不能正常的resume。
	IRQF_FORCE_RESUME	
			在系统resume的过程中，强制必须进行enable的动作，即便是设定了IRQF_NO_SUSPEND这个flag。这是和特定的硬件行为相关的。
	IRQF_NO_THREAD	
			有些low level的interrupt是不能线程化的（例如系统timer的中断），这个flag就是起这个作用的。另外，有些级联的interrupt controller对应的IRQ也是不能线程化的（例如secondary GIC对应的IRQ），
			它的线程化可能会影响一大批附属于该interrupt controller的外设的中断响应延迟。








[  878.734839]  dump_backtrace+0x0/0x168
[  878.734849]  show_stack+0x14/0x20
[  878.734860]  dump_stack+0xa8/0xcc
[  878.734871]  irq_test_handler+0xc/0x18 [irq]
[  878.734882]  __handle_irq_event_percpu+0x74/0x198
[  878.734889]  handle_irq_event_percpu+0x34/0x88
[  878.734895]  handle_irq_event+0x48/0x78
[  878.734905]  handle_edge_irq+0x104/0x228
[  878.734911]  generic_handle_irq+0x24/0x38
[  878.734923]  sunxi_pinctrl_irq_handler+0x120/0x1c0
[  878.734929]  generic_handle_irq+0x24/0x38
[  878.734935]  __handle_domain_irq+0x5c/0xb0
[  878.734942]  gic_handle_irq+0x58/0xa8
[  878.734948]  el0_irq_naked+0x4c/0x54




[   37.213022]  dump_backtrace+0x0/0x168
[   37.213031]  show_stack+0x14/0x20
[   37.213042]  dump_stack+0xa8/0xcc
[   37.213052]  dw8250_handle_irq+0x2c/0xe8
[   37.213059]  serial8250_interrupt+0x74/0x128
[   37.213068]  __handle_irq_event_percpu+0x74/0x198
[   37.213074]  handle_irq_event_percpu+0x34/0x88
[   37.213080]  handle_irq_event+0x48/0x78
[   37.213091]  handle_fasteoi_irq+0xb4/0x188
[   37.213097]  generic_handle_irq+0x24/0x38
[   37.213104]  __handle_domain_irq+0x5c/0xb0
[   37.213114]  gic_handle_irq+0x58/0xa8
[   37.213121]  el0_irq_naked+0x4c/0x54
*/








