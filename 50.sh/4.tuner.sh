#!/bin/bash




size1=`ls -l $1 | awk '{print $5}'`
size2=`ls -l $2 | awk '{print $5}'`
size3=`ls -l $3 | awk '{print $5}'`


tr '\000' '\377' < /dev/zero | dd of=fall_0xff_1.bin bs=1 count=$[696320-size1]
tr '\000' '\377' < /dev/zero | dd of=fall_0xff_2.bin bs=1 count=$[573440-size2]
tr '\000' '\377' < /dev/zero | dd of=fall_0xff_3.bin bs=1 count=$[827392-size3]


cat $1 fall_0xff_1.bin $2 fall_0xff_2.bin $3 fall_0xff_3.bin > dab.bin
rm fall_0xff_1.bin fall_0xff_2.bin fall_0xff_3.bin
