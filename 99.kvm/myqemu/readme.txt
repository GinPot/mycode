apt-get install qemu

依赖：
apt install libglib2.0-dev
apt install libpixman-1-dev
编译：
../configure --target-list=aarch64-softmmu
make
cp aarch64-softmmu/qemu-system-aarch64 ~/


dd if=/dev/zero of=test.img bs=1M count=128
mkfs.ext4 test.img

export LD_LIBRARY_PATH=/usr/local/lib/aarch64-linux-gnu/
./qemu-system-aarch64 -m 512 -cpu host -M virt -nographic -initrd work/kvm/rootfs.cpio.gz -kernel /root/boot/Image -enable-kvm -drive if=none,file=/root/test.img,id=hd0 -device virtio-blk-device,drive=hd0


===========================================================================================================================


$(CC) -c -march=armv8-a -fno-stack-protector -nostdinc -o start.o start.S

gcc start.S main.c gcc.ld
objcopy -O binary a.out guest.bin

objdump -D  a.out > xx.dump

gcc -c -o start.o start.S

gcc start.S


===========================================================================================================================


./lkvm run -k ~/work/kvm/Image -d /dev/mmcblk0p3
./lkvm run -k ~/work/kvm/qemu_linux/guest_image/Image --no-dhcp

ifconfig eth0 192.168.33.15 netmask 255.255.255.0 up;
route add default gw 192.168.33.1;
ping 192.168.33.1

ifconfig tap0 192.168.33.1