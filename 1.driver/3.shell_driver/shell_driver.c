#include <linux/module.h>
#include <linux/device.h>

#include "shell_driver.h"




static int __init shell_driver_init(void)
{
	int ret = 0;
	int i,num;
	char *envp[]	= { NULL };

//	char s_id[] = "vdo";
//	char *envp[]	= { NULL };
//	char cmd[3][25] = {"/system/bin/fsck_msdos", "-ny", "/dev/block/   "};
//	memcpy(&cmd[2][11], s_id, sizeof(s_id));
//
//	pr_info("%s will be remounting read-only, try to fsck_msdos: %s %s %s.\n", s_id, cmd[0],cmd[1],cmd[2]);
//	call_usermodehelper(cmd[0], (char **)cmd, envp, UMH_WAIT_PROC);
//	printk("%s\n", __func__);


	printk( "GinPot: %s.\n", __func__);

	num = sizeof(envpuser)/200;

	for(i=0; i<num; i++){
		//printk("GinPot: %s %s %s %s\n", envpuser[i].cmd[0],envpuser[i].cmd[1],envpuser[i].cmd[2], envpuser[i].cmd[3]);
		ret = call_usermodehelper(envpuser[i].cmd[0], envpuser[i].cmd, envp, UMH_WAIT_PROC);
		if(ret < 0){
			printk("GinPot: %s %s %s fail, ret=%d\n", envpuser[i].cmd[0],envpuser[i].cmd[1],envpuser[i].cmd[2], ret);
			return ret;
		}
		//if(i == 0)
		//	msleep(100);
	}

	printk( "GinPot: eth cfg success.\n");

	return 0;
}

static void __exit shell_driver_exit(void)
{
	printk( "GinPot: %s.\n", __func__);
}

MODULE_LICENSE("GPL");
module_init(shell_driver_init);
module_exit(shell_driver_exit);
