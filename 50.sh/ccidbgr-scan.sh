#!/bin/sh

if [ 0 == $# ]; then
    echo "usage
    $0 <cci_bus_id>
    default command is: $0 cci0";
    cci_bus="cci0";
else
    cci_bus=$1;
fi

if [ ! -e /dev/cci$cci_bus ]; then
    dev_list=`ls /dev/cci[0-9]`;
    echo -e "cci$cci_bus not found, please use below device:\n$dev_list";
    exit 1;
fi

echo "start found cci device"; 
i=0; 
cnt=0; 
while [ $i -lt 128 ]; do 
    i=$(($i+1)); 
    /bin/camera/ccidbgr "/dev/"$cci_bus $i read 0x00 1 > /dev/null 2>&1; 
    if [ 0 == $? ]; then 
        ADDR=`echo $i | awk '{printf "%x\n",$0}'`; 
        echo found cci device, address is 0x$ADDR; 
        cnt=$(($cnt+1)); 
    fi; 
done; 
echo "total found $cnt device";

