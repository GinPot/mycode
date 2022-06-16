#include <linux/module.h>
#include <linux/device.h>


/*
Linux内核使用通知链的机制在内核各子系统之间进行事件通知（注：无法用于内核态和用户态之间的事件通知）

Linux内核定义的四种通知链类型：
		通知链类型									初始化											注册函数														通知函数
Atomic notifier chains（原子通知链）			ATOMIC_NOTIFIER_HEAD()			atomic_notifier_chain_register()/atomic_notifier_chain_unregister()			atomic_notifier_call_chain()
Blocking notifier chains（可阻塞通知链）		BLOCKING_NOTIFIER_HEAD()		blocking_notifier_chain_register()/blocking_notifier_chain_unregister		blocking_notifier_call_chain()
Raw notifier chains（原始通知链）				RAW_NOTIFIER_HEAD()				raw_notifier_chain_register()/raw_notifier_chain_unregister()				raw_notifier_call_chain()
SRCU notifier chains（SRCU通知链）				srcu_init_notifier_head()		srcu_notifier_chain_register()/srcu_notifier_chain_unregister				srcu_notifier_call_chain()

1、原子通知链：通知链的回调函数在中断或原子上下文中运行，不允许阻塞。
2、可阻塞通知链：通知链的回调函数在进程上下文中运行，允许堵塞。
3、原始通知链：对通知链的回调函数没有限制，调用者维护锁和保护。
4、SRCU通知链：阻塞通知链的一个变体。
*/



static struct notifier_block *pnb = NULL;

//enum module_state {
//	MODULE_STATE_LIVE,		/* Normal state. */
//	MODULE_STATE_COMING,	/* Full formed, running module_init. */
//	MODULE_STATE_GOING,		/* Going away. */
//	MODULE_STATE_UNFORMED,	/* Still setting it up. */
//};
static char *mstate[] = {"LIVE", "COMING", "GOING"};

// notifier callback
int get_notify(struct notifier_block *p, unsigned long v, void *m)
{
    printk("module <%s> is %s, p->priority=%d\n", ((struct module *)m)->name, mstate[v], p->priority);
//	printk("module <%s> is %d, p->priority=%d\n", ((struct device *)m)->kobj.name, (unsigned int)v, p->priority);
    return 0;
}
static int module_notifier_init(void)
{
    // alloc notifier node
    pnb = kzalloc(sizeof(struct notifier_block), GFP_KERNEL);
    if(!pnb)
        return -1;
    
    pnb->notifier_call = get_notify;
    pnb->priority = 10;									//值越大，优先级越高

    //register notifier node
    register_module_notifier(pnb);						//通过module_notify_list链表，只是监听module(ko)的事件
	
//	bus_register_notifier(&platform_bus_type, pnb);		//通过platform_bus_type.p->bus_notifier链表，监听platform bus下的事件
//	bus_unregister_notifier(&platform_bus_type, pnb)
	
    printk("A listening module is coming...\n");
    return 0;
}

static void module_notifier_exit(void)
{
    unregister_module_notifier(pnb);
    kfree(pnb);
    printk("A listening module is going\n");
}
module_init(module_notifier_init);
module_exit(module_notifier_exit);




//bus类型的链表：
//#define BUS_NOTIFY_ADD_DEVICE			0x00000001 /* device added */
//#define BUS_NOTIFY_DEL_DEVICE			0x00000002 /* device to be removed */
//#define BUS_NOTIFY_REMOVED_DEVICE		0x00000003 /* device removed */
//#define BUS_NOTIFY_BIND_DRIVER		0x00000004 /* driver about to be bound */
//#define BUS_NOTIFY_BOUND_DRIVER		0x00000005 /* driver bound to device */
//#define BUS_NOTIFY_UNBIND_DRIVER		0x00000006 /* driver about to be unbound */
//#define BUS_NOTIFY_UNBOUND_DRIVER		0x00000007 /* driver is unbound from the device */
//#define BUS_NOTIFY_DRIVER_NOT_BOUND	0x00000008 /* driver fails to be bound */

