#!/bin/sh

cd /apps/newCluster/
echo "Starting cluster ..."
./AustinCluster.exe > /dev/null 2<&1&

sleep 8

# Kill and restart USB drivers without USB3. USB3 will be used for Linux guest.
#slay -s SIGKILL io-usb-otg
#slay -s SIGKILL devb-umass 
#io-usb-otg -t memory=/memory/below4G -d hcd-ehci ioport=0xEE080100,irq=0x8C,ioport=0xEE0A0100,irq=0x90,memory=/memory/below4G -d hcd-ohci ioport=0xEE080000,irq=0x8C,ioport=0xEE0A0000,irq=0x90,memory=/memory/below4G
#waitfor /dev/usb/io-usb-otg 4
#devb-umass blk cache=10m cam pnp

# GPU heap for guests that are about to go online
# QNX guest
#echo "0x60000000 0x2000000 0x620000000 0x6000000" > /tmp/guestmem

# Linux guest
echo "0x600000000 0x2000000 0x602000000 0x26000000" > /tmp/guestmem
#echo "0x90000000 0x2000000 0x92000000 0x0E000000" > /tmp/guestmem

cp /tmp/guestmem /tmp/meminfo

# Terminate device drivers for pass-through
#slay -f i2c-rcar-A
#slay -f io-usb-otg

#cd /apps/Sakura
#echo "Starting Sakura ..."
#./Sakura-20161222-BlackCar_KanziStudioProject.exe > /dev/null 2<&1&

#cd /apps/HUDApp_20171227
#echo "Starting HUDApp ..."
#./HUD_TS.exe&


cd /apps/Diagnosis/
echo "Starting cluster ..."
./Diagnosis.exe > /dev/null 2<&1&

ads1015 > /dev/null 2<&1&
grabbuffer &

# Network for guests
ifconfig tap0 create
if_up -p tap0
ifconfig tap0 up
ifconfig tap1 create
if_up -p tap1
ifconfig tap1 up
ifconfig tap2 create
if_up -p tap2
ifconfig tap2 up
ifconfig tap3 create
if_up -p tap3
ifconfig tap3 up
#ifconfig bridge0 create
brconfig bridge0 add tap0 add tap1 up add tap2 up add tap3 up

