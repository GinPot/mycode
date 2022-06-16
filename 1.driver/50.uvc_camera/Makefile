KERN_DIR = /root/env/linux-5.2/out


camerauvc-objs	:= uvc_camera.o uvc_debugfs.o uvc_ctrl.o uvc_v4l2.o uvc_video.o uvc_queue.o uvc_status.o memcpy-advsimd.o
obj-m 	= camerauvc.o



all:
	make -C $(KERN_DIR) M=$(shell pwd) modules ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
clean: 
	rm -rf *.o *.ko *.order *.mod.c *.symvers
