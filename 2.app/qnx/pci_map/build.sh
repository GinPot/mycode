#!/bin/sh
export QNX_TARGET=/disk3/wuping/hryt/tmp/apq8096au-hqx-1-0_r15.2/qnx_ap/3rdparty/qnx_bins/prebuilt_QNX700/target/qnx7
export QNX_HOST=/disk3/wuping/hryt/tmp/apq8096au-hqx-1-0_r15.2/qnx_ap/3rdparty/qnx_bins/prebuilt_QNX700/host/linux/x86_64

/disk3/wuping/hryt/tmp/apq8096au-hqx-1-0_r15.2/qnx_ap/3rdparty/qnx_bins/prebuilt_QNX700/host/linux/x86_64/usr/bin/ntoaarch64-gcc pci_map.c -I/disk3/wuping/hryt/tmp/apq8096au-hqx-1-0_r15.2/3rdparty/qnx_bins/prebuilt_QNX700/target/qnx7/usr/include/ -L/disk3/wuping/hryt/tmp/apq8096au-hqx-1-0_r15.2/3rdparty/qnx_bins/prebuilt_QNX700/target/qnx7/usr/lib -lpci -static -o pci_map
