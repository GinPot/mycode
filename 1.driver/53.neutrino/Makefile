KERN_DIR = /disk3/wuping/hryt/idcm/apq8096au-hqx-1-0_r15.2/LINUX/android/kernel/msm-4.4/build

obj-m += DWC_ETH_QOS.o

DWC_ETH_QOS-y += DWC_ETH_QOS_dev.o \
			DWC_ETH_QOS_ioctl.o \
			DWC_ETH_QOS_drv.o \
			DWC_ETH_QOS_desc.o \
			DWC_ETH_QOS_ethtool.o \
			DWC_ETH_QOS_ptp.o \
			DWC_ETH_QOS_mdio.o \
			DWC_ETH_QOS_eee.o \
			DWC_ETH_QOS_plt_init.o \
			DWC_ETH_QOS_pci.o 

PWD:=$(shell pwd) 
MAKEFLAGS += -o1 
all: 
		make -C $(KERN_DIR) M=$(PWD) modules ARCH=arm64 CROSS_COMPILE=aarch64-linux-android- 
clean: 
		rm -rf *.o *.ko *.order *.mod.c *.symvers
