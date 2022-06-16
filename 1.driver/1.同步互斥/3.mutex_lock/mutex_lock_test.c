#include <linux/module.h>
#include <linux/device.h>




static int spin_lock_test_init(void)
{
	struct mutex mutex_test;					//不能只定义指针
	mutex_init(&mutex_test);
	
	
	mutex_lock(&mutex_test);

	pr_notice("%s: mutex lock.\n", __func__);

	mutex_unlock(&mutex_test);

	pr_notice("%s: mutex unlock.\n", __func__);

	return 0;
}

static void spin_lock_test_exit(void)
{
	pr_notice("%s\n", __func__);
	mutex_destroy()
}


MODULE_LICENSE("GPL");
module_init(spin_lock_test_init);
module_exit(spin_lock_test_exit);


