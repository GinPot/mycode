#!/bin/sh

if [ 0 == $# ]; then
    echo "usage
    $0 <i2c bus>
    default command is: $0 i2c1";
    i2c_bus="i2c1";
else
    i2c_bus=$1;
fi

if [ ! -e /dev/$i2c_bus ]; then
    dev_list1=`ls /dev/i2c[0-9]`;
    echo -e "$i2c_bus not found, please use below device:\n$dev_list1";
    exit 1;
fi

echo "start found i2c device"; 
i=0; 
cnt=0; 
while [ $i -lt 128 ]; do 
    i=$(($i+1)); 
    i2cdbgr "/dev/"$i2c_bus $i read 2 0x00 1 > /dev/null 2>&1; 
    if [ 0 == $? ]; then 
        ADDR=`echo $i | awk '{printf "%x\n",$0}'`; 
        echo found i2c device, address is 0x$ADDR; 
        cnt=$(($cnt+1)); 
    fi; 
done; 
echo "total found $cnt device";