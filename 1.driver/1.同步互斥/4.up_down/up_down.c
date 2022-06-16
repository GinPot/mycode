#include <linux/module.h>
#include <linux/device.h>

/*
特点：
	1、同一时刻最多有一个写者(writer)获得锁；
	2、同一时刻可以有多个读者(reader)获得锁； 888
	3、同一时刻写者和读者不能同时获得锁；
	
	就是保证写数据的时候需要互斥；不写时，其他人读数据的时候可以同步,睡眠等待。
*/

static int up_down_test_init(void)
{
	pr_notice("%s: enter\n", __func__);
	


	return 0;
}

static void up_down_test_exit(void)
{
	pr_notice("%s\n", __func__);
}

MODULE_LICENSE("GPL");
module_init(up_down_test_init);
module_exit(up_down_test_exit);




/*
DECLARE_RWSEM(sem);
void init_rwsem(struct rw_semaphore *sem);

void down_read(struct rw_semaphore *sem);
int down_read_trylock(struct rw_semaphore *sem);
void up_read(struct rw_semaphore *sem);

void down_write(struct rw_semaphore *sem);
int down_write_trylock(struct rw_semaphore *sem);
void up_write(struct rw_semaphore *sem);

void downgrade_write(struct rw_semaphore *sem);
*/