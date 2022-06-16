#include <linux/module.h>
#include <linux/device.h>
#include <asm/sysreg.h>
#include <linux/io.h>

//void __iomem * base_addr;

static int read_cpuid_init(void)
{
	uint64_t __val;

	
	
	printk("%s\n", __func__);
	
	//id = read_sysreg_s(0x180000);

	
	asm volatile("	.irp	num,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30\n" \
"	.equ	.L__reg_num_x\\num, \\num\n"			\
"	.endr\n"						\
"	.equ	.L__reg_num_xzr, 31\n" \
"	.inst 0xd5200000|(0x180000)|(.L__reg_num_%0) "	
	: "=r" (__val));

//	asm volatile("mrs %0, MIDR_EL1 "	
//	: "=r" (__val));

//0x410fd034		
	printk("%s 7: cpuid=0x%llx\n", __func__, __val);


	//base_addr = ioremap((uint64_t)0xd5200000|(0x180000), 0x80);

	//printk("%s 7: base_addr=0x%llnx\n", __func__, (uint64_t *)base_addr);

	return 0;
}

static void read_cpuid_exit(void)
{
	printk("%s\n", __func__);
	
	//iounmap(base_addr);
}

MODULE_LICENSE("GPL");
module_init(read_cpuid_init);
module_exit(read_cpuid_exit);