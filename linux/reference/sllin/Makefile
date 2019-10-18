obj-m += sllin.o
KPATH=/home/wuping/work/m3/m3_qnx_agl_new/guest/linux/build/tmp/work-shared/salvator-x/kernel-source
#KPATH=/mnt/data/_dokumenty_/_w_/_dce_can_/src/can-benchmark/kernel/build/shark/3.0.4
#KPATH=/mnt/data/_dokumenty_/_w_/_dce_can_/src/can-benchmark/kernel/build/shark/2.6.36

all:
	make -C ${KPATH} M=$(PWD) modules

clean:
	make -C ${KPATH} M=$(PWD) clean
