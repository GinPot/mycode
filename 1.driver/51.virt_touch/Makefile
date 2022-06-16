KERN_DIR = /home/wuping/work/h5/kernel/linux-4.14


#snd-soc-ac108-objs := ac108.o




obj-m += virt_touch.o

#KERN_DIR = /home/pi/lynet/raspberrypi_src/linux		#内核所在目录
#KERN_DIR = /lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd) 

all: 
		make -C $(KERN_DIR) M=$(PWD) modules  
clean: 
		rm -rf *.o *.ko *.order *.mod.c *.symvers
