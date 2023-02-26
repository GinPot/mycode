#include <linux/delay.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>

//void my_tasklet_function(unsigned long data);
char my_tasklet_data[]="my_tasklet_function was called.";
//DECLARE_TASKLET(my_tasklet,my_tasklet_function,(unsigned long)my_tasklet_data);		//静态定义并初始化一个tasklet
//////tasklet_enable(&tasklet); //因为定义的时候使用的是DECLARE_TASKLET_DISABLED，所以需要在初始化函数（可以是.probe）时使能它
//////tasklet.data = (unsigned long)my_data; //如果需要在调用my_tasklet_func时，传递参数，可以这么做

struct tasklet_struct my_tasklet;

//tasklet任务函数
void my_tasklet_function(unsigned long data)
{
	pr_notice("%s: %s\n", __func__, (char *)data);
	//udelay(1000);	
	//tasklet_schedule(&my_tasklet);
}


static int driver_tasklet_init(void)
{
	pr_notice("%s: enter\n", __func__);

	tasklet_init(&my_tasklet,my_tasklet_function, (unsigned long )my_tasklet_data);		//动态申请


	tasklet_schedule(&my_tasklet);														//调度tasklet,一般在中断handler中调用

	return 0;
}

static void driver_tasklet_exit(void)
{
	pr_notice("%s\n", __func__);

	tasklet_kill(&my_tasklet);															//销毁tasklet
}

MODULE_LICENSE("GPL");
module_init(driver_tasklet_init);
module_exit(driver_tasklet_exit);
