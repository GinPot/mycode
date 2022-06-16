#!/bin/sh

print_qup_func() { 
    echo "---------QUP"$3"---------"; 
    echo "QUP ID    - function";
    
    i=0; 
    ADDR=$((16#$1)); 
    while [ $i -lt $2 ]; do 
        VAL=`in32 $ADDR | cut -c 21`; 
        if [ "1" == $VAL ]; then 
            FUNC="SPI"; 
        elif [ "2" == $VAL ]; then 
            FUNC="UART";
        elif [ "3" == $VAL ]; then 
            FUNC="I2C"; 
        else 
            FUNC="INVALID"; 
        fi; 
        echo "QUP"$3"_SE_"$i - $FUNC; 
        i=$(($i+1)); 
        ADDR=$(($ADDR+16384)); 
    done; 
    echo "----------------------";
}; 

# show qup0
print_qup_func 00880068 8 "0"; 
# show qup1
print_qup_func 00a80068 6 "1"; 
# show qup2
print_qup_func 00c80068 6 "2"; 
# show qup_ssc
#print_qup_func 62680068 4 "_SSC"; 