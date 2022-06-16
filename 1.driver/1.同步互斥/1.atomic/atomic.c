#include <linux/module.h>
#include <linux/device.h>


atomic_t at_tv;

static int atomic_init(void)
{

	atomic_add(3, &at_tv);pr_notice("%s: %d\n", __func__, at_tv.counter);

	atomic_sub(2, &at_tv);pr_notice("%s: %d\n", __func__, at_tv.counter);

	return 0;
}

static void atomic_exit(void)
{
	pr_notice("%s\n", __func__);
}


MODULE_LICENSE("GPL");
module_init(atomic_init);
module_exit(atomic_exit);



/*
static inline void	atomic_add(int i, atomic_t *v)							给一个原子变量v增加i
static inline int	atomic_add_return(int i, atomic_t *v)					同上，只不过将变量v的最新值返回
static inline void	atomic_sub(int i, atomic_t *v)							给一个原子变量v减去i
static inline int	atomic_sub_return(int i, atomic_t *v)					同上，只不过将变量v的最新值返回
static inline int	atomic_cmpxchg(atomic_t *ptr, int old, int new)			比较old和原子变量ptr中的值，如果相等，那么就把new值赋给原子变量,返回旧的原子变量ptr中的值

atomic_read																	获取原子变量的值
atomic_set																	设定原子变量的值
atomic_inc(v)																原子变量的值加一
atomic_inc_return(v)														同上，只不过将变量v的最新值返回
atomic_dec(v)																原子变量的值减去一
atomic_dec_return(v)														同上，只不过将变量v的最新值返回
atomic_sub_and_test(i, v)													给一个原子变量v减去i，并判断变量v的最新值是否等于0
atomic_add_negative(i,v)													给一个原子变量v增加i，并判断变量v的最新值是否是负数
static inline int atomic_add_unless(atomic_t *v, int a, int u)				只要原子变量v不等于u，那么就执行原子变量v加a的操作,如果v不等于u，返回非0值，否则返回0值
*/