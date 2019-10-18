#!/bin/bash
#	./flash_sd.sh [node] [select]
#		[node]:根据自己电脑识别SD卡节点修改(sudo fdisk -l)
#		[select]:	all全烧;
#					qnx只烧写qnx;
#					update只更新qnx、agl系统，其他分区不操作
#	如全烧：./flash_sd.sh /dev/sdc all

if [ $# != 2 ]
then
	echo "ERROR,Parameter format:"
	echo "	./flash_sd.sh [node] [select]"
	exit
fi


read -p "Please make sure the SD card device node is correct, \"$1\"? Enter (Y/N) to continue:" input
if [[ $input != Y ]] && [[ $input != y ]]; 
then
	echo "Please confirm the SD card device node!"
    exit
fi


sudo umount $1
for loop in 1 2 3 5 6
do
    sudo umount $1$loop
done


if [ $2 = "all" ]
then
	echo "start flash the whole project ..."
	
	if [ -e "hypervisor.diskimage" ]
	then
		sudo mkfs.vfat -I $1 || exit
		echo "  flash hypervisor.diskimage ..."
		sudo dd if=hypervisor.diskimage of=$1 || exit
		sudo fdisk $1 << EOF
n
p
2
1048576
9437056
n
p
3
9437184
11534304
n
e


n
11536384
12060664
n
12062720
16256960
w
EOF

if [ $? -ne 0 ]; then
    echo "fdisk failed"
	exit
fi

sudo partprobe $1 || exit

sudo umount $1
for loop in 1 2 3 5 6
do
    sudo umount $1$loop
done


		sudo mkfs.ext4 $1"2" << EOF
y
EOF
if [ $? -ne 0 ]; then
    echo "mkfs.ext4 $1\"2\" failed"
	exit
fi
		sudo mkfs.ext4 $1"3" << EOF
y
EOF
if [ $? -ne 0 ]; then
    echo "mkfs.ext4 $1\"3\" failed"
	exit
fi
		sudo mkfs.ext4 $1"5" << EOF
y
EOF
if [ $? -ne 0 ]; then
    echo "mkfs.ext4 $1\"5\" failed"
	exit
fi
		sudo mkfs.ext4 $1"6" << EOF
y
EOF
if [ $? -ne 0 ]; then
    echo "mkfs.ext4 $1\"6\" failed"
	exit
fi		

		sudo mount $1"1" /mnt/ || exit
		#sudo mkdir /mnt/guests/linux/ -p || exit
		#sudo cp hv_setup.sh linux_guest_start.sh /mnt/
		#sudo cp linux.qvmconf /mnt/guests/linux/

		if [[ -e "Image" && -e "r8a7796-salvator-xs.dtb" && -e "agl-demo-platform-salvator-x.tar.bz2" ]]
		then
			echo "flash AGL Image ..."
			sudo cp Image r8a7796-salvator-xs.dtb /mnt/disk-dos/guests/linux/  || exit
			sync
			sudo umount /mnt || exit
			
			echo "flash AGL file system ..."
			sudo mount $1"2" /mnt/ || exit
			sudo rm -rf /mnt/*  || exit
			sync
			sudo tar xvf agl-demo-platform-salvator-x.tar.bz2 -C /mnt/  || exit
			echo "sync ..."
			sync
			sudo umount /mnt || exit
			echo "****************************"
			echo "success."
			echo "****************************"
			exit
		else
		   echo "ERROR,Image or r8a7796-salvator-xs.dtb or agl-demo-platform-salvator-x.tar.bz2 miss."
		   exit
		fi

	else
	   echo "ERROR,hypervisor.diskimage miss."
	   exit
	fi	
fi

if [ $2 = "agl" ]
then
	echo "flash AGL ..."

	if [[ -e "Image" && -e "r8a7796-salvator-xs.dtb" && -e "agl-demo-platform-salvator-x.tar.bz2" ]]
	then
		sudo mount $1"1" /mnt/ || exit
		sudo cp Image r8a7796-salvator-xs.dtb /mnt/disk-dos/guests/linux/
		sync
		sudo umount /mnt || exit

		sudo mkfs.ext4 $1"3" << EOF
y
EOF
		echo "flash AGL file system ..."
		sudo mount $1"3" /mnt/ || exit
		sudo tar xvf agl-demo-platform-salvator-x.tar.bz2 -C /mnt/
		echo "sync ..."
		sync
		sudo umount /mnt || exit
		echo "****************************"
		echo "success."
		echo "****************************"
		exit
	else
	   echo "ERROR,Image or r8a7796-salvator-xs.dtb or agl-demo-platform-salvator-x.tar.bz2 miss."
	   exit
	fi
fi

if [ $2 = "update" ]
then
	echo "update start ..."

	if [[ -e "hypervisor.diskimage" && -e "agl-demo-platform-salvator-x.tar.bz2" ]]
	then
		echo "Update qnx ..."
		sudo losetup /dev/loop8 hypervisor.diskimage || exit
		sudo partprobe /dev/loop8 || exit
		mkdir ./qnx_fs || exit
		sudo mount /dev/loop8p1 ./qnx_fs || exit
		sudo mount $1"1" /mnt || exit
		sudo rm -rf /mnt/*  || exit
		sync
		sudo cp qnx_fs/* /mnt/ -rf  || exit
		sudo cp Image r8a7796-salvator-xs.dtb /mnt/disk-dos/guests/linux/ || exit
		sync
		sudo umount /mnt || exit
		sudo umount ./qnx_fs || exit
		sudo losetup -d /dev/loop8 || exit
		rm -rf ./qnx_fs || exit
		
		echo "Update AGL ..."
		sudo mkfs.ext4 $1"2" << EOF
y
EOF
		echo "Update AGL file system ..."
		sudo mount $1"2" /mnt/ || exit
		sudo rm -rf /mnt/*  || exit
		sync
		sudo tar xvf agl-demo-platform-salvator-x.tar.bz2 -C /mnt/  || exit
		echo "sync ..."
		sync
		sudo umount /mnt || exit
		echo "****************************"
		echo "success."
		echo "****************************"
		exit
	else
	   echo "ERROR,hypervisor.diskimage or agl-demo-platform-salvator-x.tar.bz2 miss."
	   exit
	fi
else
   echo "Invalid parameter."
fi
