#include <linux/module.h>
#include <linux/device.h>


static int wait_test_flag = 0;

struct workqueue_struct *WorkQueueTest;
struct work_struct test_workqueue;								//不定义指针


void workqueue_test_hardl(struct work_struct *work)
{
	wait_test_flag++;
	printk("%s: wait_test_flag=%d\n", __func__, wait_test_flag);
	queue_work(WorkQueueTest, &test_workqueue);
}



static int workqueue_test_init(void)
{

	printk( "%s: wait_test_flag=%d.\n", __func__,wait_test_flag);

	WorkQueueTest = alloc_workqueue("WorkQueueTest", WQ_UNBOUND | WQ_HIGHPRI, 0);		//创建一个workqueue
	if(!WorkQueueTest){
		printk( "%s: Alloc workqueue fail.\n", __func__);
	}
	
	INIT_WORK(&test_workqueue, workqueue_test_hardl);			//初始化一个工作项

	queue_work(WorkQueueTest, &test_workqueue);					//将工作项添加到刚创建的工作队列中，同时唤醒相应的线程处理

	return 0;
}

static void __exit workqueue_test_exit(void)
{
	printk( "GinPot: %s.\n", __func__);
	cancel_work_sync(&test_workqueue);
	flush_work(&test_workqueue);									//移除队列
	flush_workqueue(WorkQueueTest);
	if(WorkQueueTest)
		destroy_workqueue(WorkQueueTest);
}

MODULE_LICENSE("GPL");
module_init(workqueue_test_init);
module_exit(workqueue_test_exit);




/*
kworker/n:x：
	n是worker线程所在的CPU的编号；
	x是其在worker pool中的编号；
	带了"H"后缀，说明这是高优先级的worker pool；
	带"u"前缀的，它表示"unbound"，意思是这个worker线程不和任何的CPU绑定，而是被所有CPU共享，
这种设计主要是为了增加灵活性，"u"后面的这个数字也不再表示CPU的编号，
而是表示由这些unbound的worker线程组成的worker pool的ID号
*/


/*
define alloc_workqueue(fmt, flags, max_active, args...)

fmt：指 workqueue 的名称

flag：决定 workqueue 的执行模式及其 work 的处理模式(决定 workqueue 绑定那些 worker pool)，主要使用的标志如下：
	WQ_MEM_RECLAIM：如果 workqueue 的 work 相互之间有相关性，则需要设置该 flag。如果设置了这个 flag，那么工作队列在创建的时候会创建一个 救助者内核线程 备用，线程的名称为工作队列的名字。
		举例说明：假设有 A work、B work、C work、D work，当正在处理的 B work 被阻塞后，worker pool 会创建一个新的 worker thread 来处理其他的 work ，但是，在内存资源比较紧张的时候，创建 worker thread 未必能够成功。而如果此时 B work 是依赖 C或者D work 的执行结果的时候，系统进入死锁。这种状态是由于不能创建新的 worker thread 导致的。对于每一个设置 WQ_MEM_RECLAIM 的 workqueue，系统都会创建一个 rescuer thread。当发生这种情况的时候，C或者D work 会被 rescuer thread 接手处理，从而解除了死锁。
	WQ_FREEZABLE：该标志是一个和 电源管理 相关的。在系统 suspend 的时候，需要冻结用户空间的进程以及部分标注 freezable 的内核线程(包括 worker thread )。设置该标志的 workqueue 需要参与到进程冻结的过程中。其 worker thread 被冻结的时候，会处理完当前所有的 work。一旦冻结完成，那么就不会启动新的 work 的执行，直到进程被解冻。缺省情况下，所有的内核线程 都是 非freezable
	WQ_UNBOUND：设置该标志的 workqueue 说明其 work 的处理不需要绑定在特定的 CPU 上执行，workqueue 需要关联一个系统中的 unbound worker pool。如果系统中能根据 workqueue 的属性找到匹配的线程池，那么就选择一个。如果找不到适合的线程池，workqueue 就会创建一个 worker pool 来处理 work。
	WQ_HIGHPRI：说明挂入该 workqueue 的 work 是属于高优先级的 work，需要高优先级的 worker thread(一般指nice value比较低的worker)。
	WQ_CPU_INTENSIVE：该标志说明挂入 workqueue 的 work 是属于 特别消耗CPU 的任务。这类 work 会得到系统进程调度器的监管，排在这类 work 后面的 non-cpu intensive 类型 work 可能会推迟执行。也就是说设置该标志会影响并发性。
	WQ_POWER_EFFICIENT：由于 cache 的原因，per CPU 线程池 的性能会好一些，但是对功耗 有一些影响。一般情况下，workqueue 需要在 性能 和 功耗 之间平衡。想要更好的性能，最好让 per CPU上的 worker thread 来处理 work。此时 cache命中率 会比较高，性能会更好。但如果从 功耗的角度来看，最好的策略是让空闲状态的 CPU 尽可能的保持空闲，而不是反复进行 空闲-工作-空闲 的循环。
		举例说明。在 T1时刻，work 被调度到 CPU A 上执行。在 T2时刻，work 执行完毕。 CPU A 进入空闲状态。在 T3时刻， 有一个新的 work 需要处理。这时候调度 work 到 CPU A 上运行。由于之前 CPU A处理过work，其 cache 中的内容还没被刷洗掉，处理起 work 速度很快。但会如果将 CPU A 从空闲状态中唤醒。如果选择 CPU B 这个一直运行的 CPU，则不存在将 CPU 从空闲状态唤醒的操作，从而获取 功耗方面的好处。
max_active：该参数决定了 每个CPU 上分配到 workqueue的 work 的 最大数量。例如 max_active 是16，那么 同一个CPU上同一时刻最大只有16工作项可以工作。
	对于 **bound workqueue，其最大限制值为 512，并且 默认值为0 的时候则自动设置为 256。
	对于 unbound workqueue，其最大限制值为 512，并且 默认值为0 的时候则自动设置为 4。
	workqueue 中的 active work 的数量一般来说是由 用户 来进行调节的。除非一些特殊的要求需要限制工作项的数量，一般推荐 默认的0。
	一些用户需要依赖 single thread workqueue 的 严格顺序执行 的特点。那么 将其设置成1并且将flag设置成WQ_UNBOUND可以将workqueue 指定为 single thread workqueue。这类 workqueue 上的 work 总是会被放入 unbound worker pool，并且在 任意时刻都只有一个工作项被执行。这样就能够实现 类似single thread workqueue 的 相同顺序执行 的特性。
*/