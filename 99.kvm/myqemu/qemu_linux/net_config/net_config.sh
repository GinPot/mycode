#!/bin/sh


echo 1 > /proc/sys/net/ipv4/ip_forward

insmod ./net_config/nf_defrag_ipv4.ko
insmod ./net_config/nf_defrag_ipv6.ko
insmod ./net_config/libcrc32c.ko

insmod ./net_config/nf_conntrack.ko
insmod ./net_config/nf_nat.ko

insmod ./net_config/x_tables.ko
insmod ./net_config/ip_tables.ko

insmod ./net_config/iptable_nat.ko
insmod ./net_config/iptable_filter.ko
insmod ./net_config/xt_nat.ko
insmod ./net_config/xt_MASQUERADE.ko


iptables -t nat -A POSTROUTING -o usb0 -j MASQUERADE
