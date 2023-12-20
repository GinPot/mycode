#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/rcupdate.h>



struct foo{
	int a;
	int b;
	int c;
//	struct rcu_head rcu;
};
DEFINE_SPINLOCK(foo_lock);

static struct foo *gbl_foo;


struct task_struct *rcu_reader;
struct task_struct *rcu_updater;

static int rcu_reader_list(void *data)
{
	struct foo *p = NULL;
	int cnt = 100;

	while(cnt-- >= 0){
		msleep(10);
		rcu_read_lock();															//用来标识RCU read side临界区的开始，主要是disable调度
		
		p = rcu_dereference(gbl_foo);												//该接口用来获取RCU protected pointer。reader要访问RCU保护的共享数据，当然要获取RCU protected pointer，然后通过该指针进行dereference的操作
		pr_info("%s: a= %d, b = %d, c = %d \n", __func__, p->a, p->b, p->c);

		rcu_read_unlock();															//用来标识reader离开RCU read side临界区
	}
	
	return 0;
}

//timer硬件中断中检测是否有RCU任务，有则唤醒RCU软中断中处理
//void foo_reclaim(struct rcu_head *head)
//{
//	struct foo *old = container_of(head, struct foo, rcu);
//
//	kfree(old);
//}

static int rcu_updater_list(void *data)
{
	struct foo *old, *new;
	int cnt = 10;
	int value = 1000;
	
	while(cnt-- >= 0){
		msleep(100);

		new = (struct foo *)kzalloc(sizeof(struct foo), GFP_KERNEL);
		if(new == NULL){
			pr_err("%s, kzalloc fail\n", __func__);
			return -ENOMEM;
		}


		spin_lock(&foo_lock);

		old = rcu_dereference_protected(gbl_foo, lockdep_is_held(&foo_lock));		//获取老数据；应由单个写入器在写入侧(更新侧)使用，或由锁定保护，以防止多个写入器同时修改解除引用的指针。在这种情况下，指针不能在当前线程之外修改，因此不需要编译器和cpu-barrier
		*new = *old;																//基于老数据更新新数据
		new->a = value;
		new->b = value + 1;
		new->c = value + 2;	
		rcu_assign_pointer(gbl_foo, new);											//gbl_foo指针指向新的new地址； 运行完后正在读的读者读的还是老数据，后面的读者读则为新数据

		spin_unlock(&foo_lock);

		pr_info("%s: a= %d, b = %d, c = %d \n", __func__, new->a, new->b, new->c);
		
		synchronize_rcu();															//阻塞等待正在读的读者等完
		kfree(old);
//		call_rcu(&old->rcu, foo_reclaim);											//非阻塞，等待读者读完后会调用foo_reclaim释放之前用的地址，如用在softirq上下文中
		value++;
	}
	
	return 0;
}


static int rcu_test_init(void)
{
	gbl_foo = (struct foo *)kzalloc(sizeof(struct foo), GFP_KERNEL);
	if(gbl_foo == NULL){
		pr_err("%s, kzalloc fail\n", __func__);
		return -ENOMEM;
	}



	rcu_reader = kthread_run(rcu_reader_list, NULL, "rcu_reader_list");
	if (IS_ERR(rcu_reader)) {
		pr_err("%s: creat rcu_reader_list thread failed\n", __func__);
		return IS_ERR(rcu_reader);
	}

	rcu_updater = kthread_run(rcu_updater_list, NULL, "rcu_updater_list");
	if (IS_ERR(rcu_updater)) {
		pr_err("%s: creat rcu_updater_list thread failed\n", __func__);
		return IS_ERR(rcu_updater);
	}

	pr_notice("%s: init success. \n", __func__);

	return 0;
}

static void rcu_test_exit(void)
{
	pr_notice("%s\n", __func__);
}


MODULE_LICENSE("GPL");
module_init(rcu_test_init);
module_exit(rcu_test_exit);

/* 
RCU: Read Copy Update
	reader无锁访问，writer对比spin lock有些额外的操作；

RCU类型：
	1、不可抢占RCU： 过宽限期的标准是所有CPU都经过一次QS
	2、可抢占RCU： 仅要每个CPU经过一次QS，还需要每个rcu node中所记录的属于当前宽限期的blocked进程都为空
	3、SRCU: RCU读临界区允许睡眠；																				//这三种都基于CPU来检测静止态
	4、RCU-tasks:为了满足trampoline机制的需求，目前也只在bpf、ftrace等中被使用										//基于task来检测静止态

内核源码中RCU文件名可以看到:
	1、Tiny RCU： 适用于嵌入式系统单CPU的场景
	2、Tree RCU： 适用于大型多核系统




宽限期(Grace period)：
	1、普通宽限期：用户调用call_rcu()或synchronize_rcu()所开启的宽限期
	2、加速宽限期：用户调用synchronize_rcu_expedited()会触发一个加速宽限期或是将进行中的普通宽限期标记为一个加速宽限期，比普通宽限期多出一些QS的上报点，用来减少宽限期中不必要的延时。此接口会为系统带来一些额外的overhead，作者Paul建议少用
静止态(Quiescent state)：
	1、普通的QS: CPU发生进程切换，或是运行在用户态都标志着它进入了QS；
	2、Extended QS： CPU执行idle进程、进入中断、运行在用户态的tickless模式下标志着它进入EQS；
	3、Deferred QS： 调用rcu_read_unlock退出最外层RCU读临界区时若抢占是关的、或软中断是关的、或中断是关的三种情况之一都会导致读端临界区被延长，致使当前CPU的QS推迟上报；被推迟的QS会在读临界区以外的中断、软中断、进程切换中被上报
Tick中断：
	1、检查是否存在待处理的事情(如：需要上报QS、Callback过载)，有的话则唤醒RCU软中断进行实质性的工作；
	2、CPU stall的检查也是在tick中断中进行的，检测中若发现宽限期在规定的时间内(默认是21秒)未结束，则会打出cpu stall的警告
RCU软中断：	1、软中断中优先级最低；2、RT-linux中为rcu_cpu_kthread线程替代
	1、处理本CPU上已经进入DONE状态的callback；
	2、检测并更新本CPU的gp编号；检测并上报本CPU的静止态；若有需要则唤醒gp线程；
GP内核线程：rcu_gp线程； 可抢占的RCU的线程名rcu_preempt				==>5.2内核版本
	主要功能是处理宽限期相关的事务，包括：开启新的宽限期、处理forcing QS、清理已结束的宽限期


*/
/*

synchronize_rcu
  ->synchronize_rcu_expedited  																			//判断当前gp是否需要加速，若需要则调用加速接口synchronize_rcu_expedited()
  ->wait_rcu_gp(call_rcu)      																			//利用call_rcu接口注册一个名为wakeme_after_rcu的回调
    ->call_rcu(rcu_synchronize->rcu_head, wakeme_after_rcu)
	wait_for_completion(&rs_array[i].completion)														//睡眠等待，rcu软中断中会调用wakeme_after_rcu唤醒






*/

/*
callback: timer硬件中断中检测是否有RCU任务，有则唤醒RCU软中断中处理

[ 3289.870299] Call trace:
[ 3289.870318]  dump_backtrace+0x0/0x168
[ 3289.870327]  show_stack+0x14/0x20
[ 3289.870338]  dump_stack+0xa8/0xcc
[ 3289.870351]  foo_reclaim+0x14/0x28 [rcu]
rcu_do_batch(rdp);
invoke_rcu_callbacks(rdp)
[ 3289.870361]  rcu_core+0x220/0x488
[ 3289.870368]  __do_softirq+0x11c/0x23c
[ 3289.870376]  irq_exit+0xd0/0xd8
[ 3289.870384]  __handle_domain_irq+0x60/0xb0
[ 3289.870389]  gic_handle_irq+0x58/0xa8
[ 3289.870395]  el1_irq+0xb8/0x180
[ 3289.870402]  arch_cpu_idle+0x10/0x18
[ 3289.870411]  do_idle+0xcc/0x170
[ 3289.870418]  cpu_startup_entry+0x24/0x28
[ 3289.870425]  rest_init+0xd4/0xe0
[ 3289.870435]  arch_call_rest_init+0xc/0x14
[ 3289.870442]  start_kernel+0x440/0x46c
*/

/*
struct foo{
	int a;
	int b;
	int c;
	struct list_head list;
};

LIST_HEAD(g_rcu_list);

struct task_struct *rcu_reader;
struct task_struct *rcu_updater;

static int rcu_reader_list(void *data)
{
	struct foo *p = NULL;
	int cnt = 100;

	while(cnt-- >= 0){
		msleep(10);
		rcu_read_lock();
		list_for_each_entry_rcu(p, &g_rcu_list, list){
			pr_info("%s: a= %d, b = %d, c = %d \n", __func__, p->a, p->b, p->c);
		}
		rcu_read_unlock();
	}
	
	return 0;
}

static int rcu_updater_list(void *data)
{
	struct foo *old, *new;
	int cnt = 10;
	int value = 1000;
	
	while(cnt-- >= 0){
		msleep(100);
		old = list_first_or_null_rcu(&g_rcu_list, struct foo, list);
		new = (struct foo *)kzalloc(sizeof(struct foo), GFP_KERNEL);
		if(new == NULL){
			pr_err("%s, kzalloc fail\n", __func__);
			return -ENOMEM;
		}
		*new = *old;
		new->a = value;
		new->b = value + 1;
		new->c = value + 2;
		
		list_replace_rcu(&old->list, &new->list);
		
		pr_info("%s: a= %d, b = %d, c = %d \n", __func__, new->a, new->b, new->c);
		
		synchronize_rcu();
		kfree(old);
		value++;
	}
	
	return 0;
}

static int rcu_test_init(void)
{
	struct foo *p;
	
	p = (struct foo *)kzalloc(sizeof(struct foo), GFP_KERNEL);
	if(p == NULL){
		pr_err("%s, kzalloc fail\n", __func__);
		return -ENOMEM;
	}

	list_add_rcu(&p->list, &g_rcu_list);

	rcu_reader = kthread_run(rcu_reader_list, NULL, "rcu_reader_list");
	if (IS_ERR(rcu_reader)) {
		pr_err("%s: creat rcu_reader_list thread failed\n", __func__);
		return IS_ERR(rcu_reader);
	}

	rcu_updater = kthread_run(rcu_updater_list, NULL, "rcu_updater_list");
	if (IS_ERR(rcu_updater)) {
		pr_err("%s: creat rcu_updater_list thread failed\n", __func__);
		return IS_ERR(rcu_updater);
	}

	pr_notice("%s: init success. \n", __func__);

	return 0;
}

static void rcu_test_exit(void)
{
	pr_notice("%s\n", __func__);
}


MODULE_LICENSE("GPL");
module_init(rcu_test_init);
module_exit(rcu_test_exit);
*/
