KERN_DIR = /disk3/wuping/atev2020/apq8096au-hqx-1-0_r15.2/LINUX/android/kernel/msm-4.4/build


obj-m += shmem.o

PWD:=$(shell pwd) 

all: 
		make -C $(KERN_DIR) M=$(PWD) modules ARCH=arm64 CROSS_COMPILE=aarch64-linux-android-
clean: 
		rm -rf *.o *.ko *.order *.mod.c *.symvers