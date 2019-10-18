CONFIG_MODULE_SIG=n
obj-m+=qvm-shmem.o

ARCH    ?= $(shell uname -m)
KER_DIR ?= /lib/modules/$(shell uname -r)/build/

all:
	make -C $(KER_DIR) ARCH=$(ARCH) M=$(PWD) modules

clean:
	make -C $(KER_DIR) ARCH=$(ARCH) M=$(PWD) clean
