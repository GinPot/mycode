cmd_/root/work/linux/driver/50.uvc_camera/memcpy-advsimd.o := aarch64-linux-gnu-gcc -Wp,-MD,/root/work/linux/driver/50.uvc_camera/.memcpy-advsimd.o.d  -nostdinc -isystem /usr/lib/gcc/aarch64-linux-gnu/5/include -I../arch/arm64/include -I./arch/arm64/include/generated -I../include -I./include -I../arch/arm64/include/uapi -I./arch/arm64/include/generated/uapi -I../include/uapi -I./include/generated/uapi -include ../include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -DKASAN_SHADOW_SCALE_SHIFT=3 -D__ASSEMBLY__ -fno-PIE -DCONFIG_AS_LSE=1 -mabi=lp64 -DKASAN_SHADOW_SCALE_SHIFT=3 -Wa,-gdwarf-2 -DMODULE  -c -o /root/work/linux/driver/50.uvc_camera/memcpy-advsimd.o /root/work/linux/driver/50.uvc_camera/memcpy-advsimd.S

source_/root/work/linux/driver/50.uvc_camera/memcpy-advsimd.o := /root/work/linux/driver/50.uvc_camera/memcpy-advsimd.S

deps_/root/work/linux/driver/50.uvc_camera/memcpy-advsimd.o := \
  ../include/linux/kconfig.h \
    $(wildcard include/config/cpu/big/endian.h) \
    $(wildcard include/config/booger.h) \
    $(wildcard include/config/foo.h) \
  /root/work/linux/driver/50.uvc_camera/asmdefs.h \

/root/work/linux/driver/50.uvc_camera/memcpy-advsimd.o: $(deps_/root/work/linux/driver/50.uvc_camera/memcpy-advsimd.o)

$(deps_/root/work/linux/driver/50.uvc_camera/memcpy-advsimd.o):
