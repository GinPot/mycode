#!/bin/bash

_GL=e6055400.gpio
_PIN=13
_GC=
for gcn in /sys/class/gpio/gpiochip*
do
  _NAM=`cat ${gcn}/label`
  if [ $? -ne 0 ]
  then
    echo "Error scanning gpiochip labels" 1>&2
    exit 1
  fi
  if [ "${_NAM}" == "$_GL" ]
  then
    _GC=`echo ${gcn} | sed 's/[^0-9]*//g'`
  fi
done
if [ -z $_GC ]
then
  echo "could not find gpio with label ${_GL}" 1>&2
  exit 1
fi

export PIN=$[$_GC + $_PIN]
export GPF=/sys/class/gpio/gpio${PIN}

if ! [ -d ${GPF} ]
then
  echo ${PIN}
  echo ${PIN} > /sys/class/gpio/export
fi

ln -s ${GPF} /run/gpio_tunerreset

echo out > ${GPF}/direction
echo 0 > ${GPF}/active_low
