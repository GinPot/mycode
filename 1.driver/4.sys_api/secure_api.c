#include <linux/module.h>
#include <linux/device.h>

#include "secure_api.h"

static struct class *secure_api_class;
static struct device *secure_api_dev;



static ssize_t secure_get_value(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", 1);
}

static DEVICE_ATTR(secure_api, S_IRUGO, secure_get_value, NULL);

static int misc_init(void){

	secure_api_class = class_create(THIS_MODULE, "secure_api_class");
	secure_api_dev = device_create(secure_api_class, NULL, 0, NULL, "%s","secure_api_dev");
	if(IS_ERR(secure_api_dev))
		return PTR_ERR(secure_api_dev);

	device_create_file(secure_api_dev, &dev_attr_secure_api);	//创建属性文件


    return 0;
}

static void misc_exit(void){
	device_remove_file(secure_api_dev, &dev_attr_secure_api);
	device_destroy(secure_api_class, secure_api_dev->devt);
	class_destroy(secure_api_class);
}

MODULE_LICENSE("GPL");
module_init(misc_init);
module_exit(misc_exit);