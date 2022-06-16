#!/bin/bash
source ../build/qnx700/qnxsdp-env.sh  || exit
BASE=$(pwd)
export BASE

cd images/host_bsp/  || exit
make clean  || exit
make  || exit
cd ../../  || exit
make clean  || exit
make  || exit