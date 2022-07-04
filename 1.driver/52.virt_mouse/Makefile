KERN_DIR = /home/wuping/work/m3/m3_qnx_agl_new/guest/linux/build/tmp/work-shared/salvator-x/kernel-build-artifacts


#snd-soc-ac108-objs := ac108.o




obj-m += virt_mouse.o

#KERN_DIR = /home/pi/lynet/raspberrypi_src/linux		#内核所在目录
#KERN_DIR = /lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd) 

all: 
		make -C $(KERN_DIR) M=$(PWD) modules  
clean: 
		rm -rf *.o *.ko *.order *.mod.c *.symvers .*
