#ccflags-y += -g -DDEBUG

obj-m := virtio_audio.o
virtio_audio-objs += snd_virtaudio.o virtaudio.o

CROSS ?= /home/pma/disk1/wuping/dc1e/dayin/qcom-auto-gen3-hqx-1-0-r19.1-post-cs3/lagvm_p/LINUX/android/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-androidkernel-
KERNEL ?= /home/pma/disk1/wuping/dc1e/dayin/qcom-auto-gen3-hqx-1-0-r19.1-post-cs3/lagvm_p/LINUX/android/kernel/msm-4.14/build

all:
ifeq ($(ARCH),arm64)
	make CROSS_COMPILE=$(CROSS) -C $(KERNEL) M=$(PWD) modules
else
	make -C $(KERNEL) M=$(PWD) modules
endif

clean:
	make -C $(KERNEL) M=$(PWD) clean
	rm -f *.o.ur-safe

help:
	@echo
	@echo "  make [target] [OPTIONS]"
	@echo
	@echo " Targets:"
	@echo "     all             Builds the module for host architecture (default)."
	@echo "     clean           Clean all the binaries."
	@echo "     help            Prints this message."
	@echo " Options:"
	@echo "     ARCH=arm64      Build an AARCH64 build"
	@echo "     CROSS=<dir>     Path to AARCH64 cross compiler"
	@echo "     KERNEL=<dir>    Path to Linux kernel headers"
	@echo