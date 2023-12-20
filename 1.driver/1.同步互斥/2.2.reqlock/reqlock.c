#include <linux/module.h>
#include <linux/device.h>


static DEFINE_SEQLOCK(seqlock_test);		//初始化方案1

static int rwspinlock_test_init(void)
{
//	seqlock_t  seqlock_test;					//不能只定义指针
//	seqlock_init(&seqlock_test);				//初始化方案2
	
	
	write_seqlock(&seqlock_test);

	pr_notice("%s: seqlock lock.\n", __func__);

	write_sequnlock(&seqlock_test);

	pr_notice("%s: seqlock unlock.\n", __func__);


//	unsigned long seq;
//	 
//	do { 
//	     seq = read_seqbegin(&seqlock); 
//	/* ---------- 这里读临界区数据 ----------*/
//	} while (read_seqretry(&seqlock, seq));		//确定当前没有writer，没有的话就正常往下，有则重新读取

	return 0;
}

static void rwspinlock_test_exit(void)
{
	pr_notice("%s\n", __func__);
}


MODULE_LICENSE("GPL");
module_init(rwspinlock_test_init);
module_exit(rwspinlock_test_exit);


/*
seqlock(顺序自旋锁): 	
	1、顺序锁临界区只允许一个writer thread进入（在多个写者之间是互斥的），临界区只允许一个writer thread进入，在没有writer thread的情况下，reader thread可以随意进入;
	2、在临界区只有有reader thread的情况下，writer thread可以立刻执行，不会等待；

（1）read操作比较频繁
（2）write操作较少，但是性能要求高，不希望被reader thread阻挡



==========
Writer thread的操作：

对于writer thread，获取seqlock操作如下：

（1）获取锁（例如spin lock），该锁确保临界区只有一个writer进入。
（2）sequence counter加一

释放seqlock操作如下：

（1）释放锁，允许其他writer thread进入临界区。
（2）sequence counter加一（注意：不是减一哦，sequence counter是一个不断累加的counter）

由上面的操作可知，如果临界区没有任何的writer thread，那么sequence counter是偶数（sequence counter初始化为0），如果临界区有一个writer thread（当然，也只能有一个），那么sequence counter是奇数。

 

Reader thread的操作如下：

（1）获取sequence counter的值，如果是偶数，可以进入临界区，如果是奇数，那么等待writer离开临界区（sequence counter变成偶数）。进入临界区时候的sequence counter的值我们称之old sequence counter。
（2）进入临界区，读取数据
（3）获取sequence counter的值，如果等于old sequence counter，说明一切OK，否则回到step（1）



*/