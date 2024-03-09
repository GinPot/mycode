#include <linux/module.h>
#include <linux/device.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>



struct gpioData {
	struct device *dev;
	int gpio;
};




static ssize_t led_gpio_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct gpioData *gpiodata = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", gpio_get_value(gpiodata->gpio));
}
static ssize_t led_gpio_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	int ret;
	unsigned long value;
	struct gpioData *gpiodata = dev_get_drvdata(dev);

	ret = kstrtoul(buf, 10, &value);
	if (ret < 0 || value > 0xFF)
		return -EINVAL;

	gpio_set_value(gpiodata->gpio, value);

	return size;
}

// /sys/devices/platform/gpio_testfs/led_gpio
static struct device_attribute led_gpio_attributes[] = {
	__ATTR(led_gpio, 0775, led_gpio_show, led_gpio_store),
};

static int gpio_test_probe(struct platform_device *pdev)
{
	int rc = 0;
	struct gpioData *gpiodata = NULL;
	
	dev_info(&pdev->dev, "%s %d: enter\n", __func__, __LINE__);

	gpiodata = devm_kzalloc(&pdev->dev, sizeof(*gpiodata), GFP_KERNEL);
	if(!gpiodata){
		dev_err(&pdev->dev, "%s %d: out of memory\n", __func__, __LINE__);
		rc = -ENOMEM;
		goto err;
	}
	gpiodata->dev = &pdev->dev;
	platform_set_drvdata(pdev, gpiodata);

	gpiodata->gpio = of_get_named_gpio(pdev->dev.of_node, "led_gpio", 0);
	if (!gpio_is_valid(gpiodata->gpio)) {
		dev_err(&pdev->dev, "%s %d: gpio is invalid\n", __func__, __LINE__);
		rc = -ENXIO;
		goto err_free;
	}

	rc = gpio_request(gpiodata->gpio, "gpio-testfs");							//申请一个gpio脚
	if (rc) {
		dev_err(&pdev->dev, "%s: unable to request gpio %d (%d)\n", __func__, gpiodata->gpio, rc);
		goto err_free;
	}
	rc = gpio_direction_output(gpiodata->gpio, 0);								//将这个gpio设置为输出模式，且默认输出低电平; 相关函数：gpio_direction_input(gpio)
	if (rc) {
		dev_err(&pdev->dev, "%s: Unable to set direction\n", __func__);
		goto err_gpio;
	}
	
	//gpio_set_value(gpiodata->gpio, 1);

	if (device_create_file(&pdev->dev, led_gpio_attributes)){
		dev_err(&pdev->dev, "%s: device create file fail\n", __func__);
		goto err_gpio;
	}

	dev_info(&pdev->dev, "led_gpio(%d) value(%d)\n", gpiodata->gpio, gpio_get_value(gpiodata->gpio));


	return rc;


err_gpio:
	gpio_free(gpiodata->gpio);
err_free:
	//kfree(gpiodata);
err:
	return rc;
}


static int gpio_test_remove(struct platform_device *pdev)
{
	struct gpioData *gpiodata = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "%s %d: enter\n", __func__, __LINE__);

	device_remove_file(&pdev->dev, led_gpio_attributes);

	gpio_set_value(gpiodata->gpio, 0);
	gpio_free(gpiodata->gpio);
	//kfree(gpiodata);


	return 0;
}

static const struct of_device_id gpio_dt_match[] = {
	{ .compatible = "gpio-testfs", },
	{ },
};

static struct platform_driver gpio_test_driver = {
	.driver = {
		.name			= "gpio-testfs",
		.of_match_table	= gpio_dt_match,
	},
	.probe	= gpio_test_probe,
	.remove	= gpio_test_remove,
};

module_platform_driver(gpio_test_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Allwinner H5 gpio test");
MODULE_AUTHOR("GinPot <GinPot@xxx.com>");


/*
r_pio: pinctrl@1f02c00 {
	...
	gpio_pins: gpio-pins {
		pins = "PL10";
		function = "gpio_out";
		drive-strength = <40>;
		bias-pull-up;
	};
};

gpio_testfs {
	compatible = "gpio-testfs";

	pinctrl-names = "default";				//probe前设置gpio的复用功能、上下拉、驱动电流等参数
	pinctrl-0 = <&gpio_pins>;

	led_gpio = <&r_pio 0 10 GPIO_ACTIVE_HIGH>;
};
*/