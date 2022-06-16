KERNER_DIR 	:= /root/env/linux-5.2/out/
ENV			:= ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-

#SRC_C		:= $(wildcard *.c)
#obj-m 		:= $(SRC_C:%.c=%.o)
obj-m 		:= mutex_lock_test.o

all:
	make -C $(KERNER_DIR) M=$(shell pwd) modules $(ENV)
clean:
	rm -rf *.o *.ko *.order *.mod.c *.symvers