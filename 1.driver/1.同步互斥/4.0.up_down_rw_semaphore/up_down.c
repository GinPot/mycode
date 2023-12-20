#include <linux/module.h>
#include <linux/device.h>

/*
相当于睡眠版rwspinlock(rcu更优秀)
特点：
	1、同一时刻最多有一个写者(writer)获得锁；
	2、同一时刻可以有多个读者(reader)获得锁； 888
	3、同一时刻写者和读者不能同时获得锁；
	
	就是保证写数据的时候需要互斥；不写时，其他人读数据的时候可以同步,睡眠等待。
*/
#define EXEC_DOWN_WRITE 0
struct rw_semaphore rwsem;

static int up_down_test_init(void)
{
	init_rwsem(&rwsem);                  //读写信号量初始化


	pr_notice("after init_rwsem, count: %ld\n", rwsem.count.counter);

    if(EXEC_DOWN_WRITE)
        down_write(&rwsem);            	 //写者获取读写信号量

	down_read(&rwsem);                   //读者获取读写信号量,  EXEC_DOWN_WRITE为1的情况下，因为前期有写者，运行到这会睡眠等待
	pr_notice("first down_read, count: %ld\n", rwsem.count.counter);
	down_read(&rwsem);
	pr_notice("second down_read, count: %ld\n", rwsem.count.counter);

    while(rwsem.count.counter != 0)
        up_read(&rwsem);                //读者释放读写信号量
	
	pr_notice("after up_read, count: %ld\n", rwsem.count.counter);

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
第一步定义宏EXEC_DOWN_WRITE为0，则在执行down_read()操作之前读写信号量rwsem没有被写者获取，读者可以成功获得信号量。
每个读者获取信号量时，count值都会递增1表示持有信号量的读者个数（起初count由init_rwsem()初始化为0）。
最后程序调用up_read()释放读写信号量，它对count执行递减1操作。


第二步定义宏EXEC_DOWN_WRITE为1，则在执行down_read()操作之前读写信号量rwsem已经被写者获取，读者将不能获得信号量，
因此插入模块后进程将被挂起。
*/

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