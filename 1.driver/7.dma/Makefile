KERN_DIR = /root/env/linux-5.2/out

SRC_C	:= $(wildcard *.c)
SRC_O 	:= $(SRC_C:%.c=%.o)
obj-m 	+= dma_test_kzalloc.o



all: 
		make -C $(KERN_DIR) M=$(shell pwd) modules ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
clean: 
		rm -rf *.o *.ko *.order *.mod.c *.symvers

