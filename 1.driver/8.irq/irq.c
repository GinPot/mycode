static int cyttsp6_setup_irq_gpio(struct cyttsp6_core_data *cd)
{
	struct device *dev = cd->dev;
	unsigned long irq_flags;
	int rc;
	const struct cpumask *mask;

	/* Initialize IRQ */
	//cd->irq = of_irq_get_byname(dev->of_node, "tsc_irq");
	of_property_read_u32(dev->of_node, "tp-irq-gpio", &cd->irq_gpio);
	if (!cd->irq_gpio)
		return -EINVAL;
	dev_info(dev, "tp-irq-gpio=%d\n", cd->irq_gpio);

    rc = gpio_request(cd->irq_gpio,"tp-irq-gpio");  
    if( rc != 0){
        printk("gpio request failed\n");
        return -1;
    }

    //将gpio设置为输入
    gpio_direction_input(cd->irq_gpio);
	
	cd->irq = gpio_to_irq(cd->irq_gpio);
	if (cd->irq < 0)
		return -EINVAL;


	dev_dbg(dev, "%s: initialize threaded irq=%d\n", __func__, cd->irq);
	if (cd->cpdata->level_irq_udelay > 0)
		/* use level triggered interrupts */
		//irq_flags = IRQF_TRIGGER_LOW | IRQF_ONESHOT;
		irq_flags = IRQF_TRIGGER_HIGH | IRQF_ONESHOT;
	else
		/* use edge triggered interrupts */
		//irq_flags = IRQF_TRIGGER_FALLING | IRQF_ONESHOT;
		irq_flags = IRQF_TRIGGER_HIGH | IRQF_ONESHOT;

	rc = request_threaded_irq(cd->irq, cyttsp6_hard_irq, cyttsp6_irq,
			irq_flags, dev_name(dev), cd);
	if (rc < 0)
		dev_err(dev, "%s: Error, could not request irq\n", __func__);

	

	mask = get_cpu_mask(7);
	rc = irq_set_affinity_hint(cd->irq, mask);
	if (rc) {
		dev_err(dev, "Failed to set cpu affinity for ethernet irq:%d\n",
			cd->irq);
	} else {
		dev_info(dev, "CPU affinity hint set for ethernet irq:%d\n",
				cd->irq);
	}

	
	
	return rc;
}




/*
IRQF_DISABLED	
		首先要说明的是这是一个废弃的flag，在新的内核中，该flag没有任何的作用了。具体可以参考：Disabling IRQF_DISABLED
		旧的内核（2.6.35版本之前）认为有两种interrupt handler：slow handler和fast handle。在request irq的时候，对于fast handler，需要传递IRQF_DISABLED的参数，确保其中断处理过程中是关闭CPU的中断，
		因为是fast handler，执行很快，即便是关闭CPU中断不会影响系统的性能。但是，并不是每一种外设中断的handler都是那么快（例如磁盘），因此就有 slow handler的概念，说明其在中断处理过程中会耗时比较长。
		对于这种情况，在执行interrupt handler的时候不能关闭CPU中断，否则对系统的performance会有影响。
		新的内核已经不区分slow handler和fast handle，都是fast handler，都是需要关闭CPU中断的，那些需要后续处理的内容推到threaded interrupt handler中去执行。
IRQF_SHARED	
		这是flag用来描述一个interrupt line是否允许在多个设备中共享。如果中断控制器可以支持足够多的interrupt source，那么在两个外设间共享一个interrupt request line是不推荐的，毕竟有一些额外的开销（发生中断的时候要逐个询问是不是你的中断，软件上就是遍历action list），因此外设的irq handler中最好是一开始就启动判断，看看是否是自己的中断，如果不是，返回IRQ_NONE,表示这个中断不归我管。 早期PC时代，使用8259中断控制器，级联的8259最多支持15个外部中断，但是PC外设那么多，因此需要irq share。现在，ARM平台上的系统设计很少会采用外设共享IRQ方式，毕竟一般ARM SOC提供的有中断功能的GPIO非常的多，足够用的。 当然，如果确实需要两个外设共享IRQ，那也只能如此设计了。对于HW，中断控制器的一个interrupt source的引脚要接到两个外设的interrupt request line上，怎么接？直接连接可以吗？当然不行，对于低电平触发的情况，我们可以考虑用与门连接中断控制器和外设。

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
		
		一般用在中断现成化时，且电平触发中断；线程化时表明下半部比较耗时，因为上半部可能会恒快结束并使能中断导致中断又触发，所以加上这个标志等下半部处理完后再使能中断
		
IRQF_NO_SUSPEND	
		这个flag比较好理解，就是说在系统suspend的时候，不用disable这个中断，如果disable，可能会导致系统不能正常的resume。
IRQF_FORCE_RESUME	
		在系统resume的过程中，强制必须进行enable的动作，即便是设定了IRQF_NO_SUSPEND这个flag。这是和特定的硬件行为相关的。
IRQF_NO_THREAD	
		有些low level的interrupt是不能线程化的（例如系统timer的中断），这个flag就是起这个作用的。另外，有些级联的interrupt controller对应的IRQ也是不能线程化的（例如secondary GIC对应的IRQ），
		它的线程化可能会影响一大批附属于该interrupt controller的外设的中断响应延迟。
*/








