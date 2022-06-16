#!/bin/sh
export QNX_TARGET=/disk3/wuping/hryt/tmp/apq8096au-hqx-1-0_r15.2/qnx_ap/3rdparty/qnx_bins/prebuilt_QNX700/target/qnx7
export QNX_HOST=/disk3/wuping/hryt/tmp/apq8096au-hqx-1-0_r15.2/qnx_ap/3rdparty/qnx_bins/prebuilt_QNX700/host/linux/x86_64

/disk3/wuping/hryt/tmp/apq8096au-hqx-1-0_r15.2/qnx_ap/3rdparty/qnx_bins/prebuilt_QNX700/host/linux/x86_64/usr/bin/ntoaarch64-gcc eth_test.c -I/disk3/zzy/qnx_ap_hqx1.0_r10.1/3rdparty/qnx_bins/prebuilt_QNX700/target/qnx7/usr/include/ -L/disk3/zzy/qnx_ap_hqx1.0_r10.1/3rdparty/qnx_bins/prebuilt_QNX700/target/qnx7/usr/lib -lsocket -static -o 88Q5050PHY
