// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Simple synchronous userspace interface to SPI devices
 *
 * Copyright (C) 2006 SWAPP
 *	Andrea Paterniani <a.paterniani@swapp-eng.it>
 * Copyright (C) 2007 David Brownell (simplification, cleanup)
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/acpi.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>

#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

#include <linux/uaccess.h>





static ssize_t cs_si47961_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct spidev_data *spidev = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", gpio_get_value(spidev->cs_si47961));
}
static ssize_t cs_si47961_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	int ret;
	unsigned long value;
	struct spidev_data *spidev = dev_get_drvdata(dev);

	ret = kstrtoul(buf, 10, &value);
	if (ret < 0 || value > 0xFF)
		return -EINVAL;

	gpio_set_value(spidev->cs_si47961, value);

	return size;
}

static ssize_t cs_si4692_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct spidev_data *spidev = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", gpio_get_value(spidev->cs_si4692));
}
static ssize_t cs_si4692_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	int ret;
	unsigned long value;
	struct spidev_data *spidev = dev_get_drvdata(dev);

	ret = kstrtoul(buf, 10, &value);
	if (ret < 0 || value > 0xFF)
		return -EINVAL;

	gpio_set_value(spidev->cs_si4692, value);

	return size;
}

static ssize_t cs_si47962_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct spidev_data *spidev = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", gpio_get_value(spidev->cs_si47962));
}
static ssize_t cs_si47962_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	int ret;
	unsigned long value;
	struct spidev_data *spidev = dev_get_drvdata(dev);

	ret = kstrtoul(buf, 10, &value);
	if (ret < 0 || value > 0xFF)
		return -EINVAL;

	gpio_set_value(spidev->cs_si47962, value);

	return size;
}

static struct device_attribute spi_cs_attributes[] = {
	__ATTR(cs_si47961, 0775, cs_si47961_show, cs_si47961_store),
	__ATTR(cs_si4692, 0775, cs_si4692_show, cs_si4692_store),
	__ATTR(cs_si47962, 0775, cs_si47962_show, cs_si47962_store),
};

static int spidev_cs_init(struct spidev_data *spidev)
{
	int rc = 0, i;

	spidev->cs_si47961 = of_get_named_gpio(spidev->spi->dev.of_node, "spi,cs-si47961", 0);
	if (spidev->cs_si47961 < 0){
		pr_warn("spi,cs-si47961 not provided in devicetree\n");
		return -EINVAL;
	}
	spidev->cs_si4692 = of_get_named_gpio(spidev->spi->dev.of_node, "spi,cs-si4692", 0);
	if (spidev->cs_si4692 < 0){
		pr_warn("spi,cs-si4692 not provided in devicetree\n");
		return -EINVAL;
	}
	spidev->cs_si47962 = of_get_named_gpio(spidev->spi->dev.of_node, "spi,cs-si47962", 0);
	if (spidev->cs_si47962 < 0){
		pr_warn("spi,cs-si47962 not provided in devicetree\n");
		return -EINVAL;
	}

	rc = gpio_request(spidev->cs_si47961, "cs_si47961");
	if (rc) {
		pr_err("%s: unable to request gpio %d (%d)\n", __func__, spidev->cs_si47961, rc);
		return rc;
	}
	rc = gpio_direction_output(spidev->cs_si47961, 0);
	if (rc) {
		pr_err("%s: Unable to set direction\n", __func__);
		return rc;
	}
	pr_info("cs_si47961(%d) value(%d)\n", spidev->cs_si47961, gpio_get_value(spidev->cs_si47961));

	rc = gpio_request(spidev->cs_si4692, "cs_si4692");
	if (rc) {
		pr_err("%s: unable to request gpio %d (%d)\n", __func__, spidev->cs_si4692, rc);
		return rc;
	}
	rc = gpio_direction_output(spidev->cs_si4692, 0);
	if (rc) {
		pr_err("%s: Unable to set direction\n", __func__);
		return rc;
	}
	pr_info("cs_si4692(%d) value(%d)\n", spidev->cs_si4692, gpio_get_value(spidev->cs_si4692));

	rc = gpio_request(spidev->cs_si47962, "cs_si47962");
	if (rc) {
		pr_err("%s: unable to request gpio %d (%d)\n", __func__, spidev->cs_si47962, rc);
		return rc;
	}
	rc = gpio_direction_output(spidev->cs_si47962, 0);
	if (rc) {
		pr_err("%s: Unable to set direction\n", __func__);
		return rc;
	}
	pr_info("cs_si47962(%d) value(%d)\n", spidev->cs_si47962, gpio_get_value(spidev->cs_si47962));


	for (i = 0; i < ARRAY_SIZE(spi_cs_attributes); i++)
		if (device_create_file(&spidev->spi->dev, spi_cs_attributes + i))
			goto undo;

	return 0;

undo:
	for (i--; i >= 0; i--)
		device_remove_file(&spidev->spi->dev, spi_cs_attributes + i);
	dev_err(&spidev->spi->dev, "%s: failed to create sysfs interface\n", __func__);
	return -ENODEV;
}


static int spidev_remove(struct spi_device *spi)
{
	int i;
	struct spidev_data	*spidev = spi_get_drvdata(spi);


	gpio_free(spidev->cs_si47961);
	gpio_free(spidev->cs_si4692);
	gpio_free(spidev->cs_si47962);
	for (i = 0; i < ARRAY_SIZE(spi_cs_attributes); i++)
		device_remove_file(&spidev->spi->dev, spi_cs_attributes + i);



	return 0;
}


/*
qupv3_se8_spi_pins_cs: qupv3_se8_spi_pins_cs {
        qupv3_se8_spi_cs_active: qupv3_se8_spi_cs_active {
                mux {
                        pins = "gpio91", "gpio94", "gpio95";
                        function = "gpio";
                };

                config {
                        pins = "gpio91", "gpio94", "gpio95";
                        drive-strength = <6>;
                        bias-disable;
                        output-low;
                };
        };
};
//TUNER
&qupv3_se8_spi {
    status = "ok";
    tuner@0 {
        compatible = "yfve,tuner";
        reg = <0>;
        spi-max-frequency = <10000000>;
        //spi-cpha;

        pinctrl-names = "default";
        pinctrl-0 = <&qupv3_se8_spi_cs_active>;
        spi,cs-si47961 = <&tlmm 91 0>;
        spi,cs-si4692  = <&tlmm 94 0>;
        spi,cs-si47962 = <&tlmm 95 0>;
    };
};
*/