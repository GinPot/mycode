#ifndef __SHELL_DRIVER_H__
#define __SHELL_DRIVER_H__



struct envp {
	char *cmd[25];
};

struct envp envpuser[] = {
	{ .cmd = { "/system/bin/insmod", "/vendor/lib/modules/DWC_ETH_QOS.ko", NULL } },

	{ .cmd = { "/system/bin/ifconfig", "eth1", "up", NULL } },
	{ .cmd = { "/system/bin/ifconfig", "eth2", "up", NULL } },

	{ .cmd = { "/system/bin/brctl", "addbr", "br0", NULL } },
	{ .cmd = { "/system/bin/brctl", "addif", "br0", "eth1", "eth2", NULL } },
	{ .cmd = { "/system/bin/ifconfig", "br0", "up", NULL } },

	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "100", NULL } },
	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "101", NULL } },
	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "102", NULL } },
	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "103", NULL } },
	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "104", NULL } },
	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "110", NULL } },

	{ .cmd = { "/system/bin/ifconfig", "br0.100", "up", NULL } },
	{ .cmd = { "/system/bin/ifconfig", "br0.101", "up", NULL } },
	{ .cmd = { "/system/bin/ifconfig", "br0.102", "up", NULL } },
	{ .cmd = { "/system/bin/ifconfig", "br0.103", "up", NULL } },
	{ .cmd = { "/system/bin/ifconfig", "br0.104", "up", NULL } },
	{ .cmd = { "/system/bin/ifconfig", "br0.110", "up", NULL } },



/*
echo "1001 eth0" >> /data/misc/net/rt_tables
echo "1002 eth1" >> /data/misc/net/rt_tables 
echo "1004 defa_netmask" >> /data/misc/net/rt_tables 
echo "1005 usb0" >> /data/misc/net/rt_tables 
echo 1 > /proc/sys/net/ipv4/ip_forward
*/
//	{ .cmd = { "/system/bin/insmod", "/data/DWC_ETH_QOS.ko", NULL } },
//	{ .cmd = { "/system/bin/ifconfig", "eth0", "223.255.255.2", "netmask", "255.255.255.252", "up", NULL } },
//	{ .cmd = { "/system/bin/ifconfig", "eth1", "up", NULL } },
//	{ .cmd = { "/system/bin/ifconfig", "eth2", "up", NULL } },
//
//	{ .cmd = { "/system/bin/brctl", "addbr", "br0", NULL } },
//	{ .cmd = { "/system/bin/brctl", "addif", "br0", "eth1", "eth2", NULL } },
//	{ .cmd = { "/system/bin/ifconfig", "br0", "up", NULL } },
//
//	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "100", NULL } },
//	{ .cmd = { "/system/bin/ifconfig", "br0.100", "192.168.0.4", "netmask", "255.255.255.0", "up", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.100", "0", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.100", "1", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.100", "2", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.100", "3", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.100", "4", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.100", "5", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.100", "6", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.100", "7", "0", NULL } },
//
//	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "101", NULL } },
//	{ .cmd = { "/system/bin/ifconfig", "br0.101", "192.168.1.4", "netmask", "255.255.255.0", "up", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.101", "0", "1", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.101", "1", "1", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.101", "2", "1", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.101", "3", "1", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.101", "4", "1", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.101", "5", "1", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.101", "6", "1", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.101", "7", "1", NULL } },
//
//	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "102", NULL } },
//	{ .cmd = { "/system/bin/ifconfig", "br0.102", "192.168.2.4", "netmask", "255.255.255.0", "up", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.102", "0", "2", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.102", "1", "2", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.102", "2", "2", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.102", "3", "2", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.102", "4", "2", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.102", "5", "2", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.102", "6", "2", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.102", "7", "2", NULL } },
//
//	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "103", NULL } },
//	{ .cmd = { "/system/bin/ifconfig", "br0.103", "192.168.3.4", "netmask", "255.255.255.0", "up", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.103", "0", "3", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.103", "1", "3", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.103", "2", "3", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.103", "3", "3", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.103", "4", "3", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.103", "5", "3", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.103", "6", "3", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.103", "7", "3", NULL } },
//
//	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "104", NULL } },
//	{ .cmd = { "/system/bin/ifconfig", "br0.104", "192.168.4.4", "netmask", "255.255.255.0", "up", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.104", "0", "4", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.104", "1", "4", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.104", "2", "4", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.104", "3", "4", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.104", "4", "4", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.104", "5", "4", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.104", "6", "4", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.104", "7", "4", NULL } },
//
//	{ .cmd = { "/system/bin/toybox", "vconfig", "add", "br0", "110", NULL } },
//	{ .cmd = { "/system/bin/ifconfig", "br0.110", "192.168.10.4", "netmask", "255.255.255.0", "up", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.110", "0", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.110", "1", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.110", "2", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.110", "3", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.110", "4", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.110", "5", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.110", "6", "0", NULL } },
//	{ .cmd = { "/system/bin/toybox", "vconfig", "set_egress_map", "br0.110", "7", "0", NULL } },
//
//	{ .cmd = { "/system/bin/iptables", "-t", "nat", "-A", "POSTROUTING", "-s", "192.168.43.0/24", "-o", "br0.110", "-j", "SNAT", "--to-source", "192.168.10.4", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-t", "nat", "-A", "POSTROUTING", "-d", "192.168.43.0/24", "-o", "softap0", "-j", "MASQUERADE", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-A", "oem_fwd", "-s", "192.168.43.0/24", "-o", "br0.110", "-j", "ACCEPT", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-A", "oem_fwd", "-d", "192.168.43.0/24", "-o", "softap0", "-j", "ACCEPT", NULL } },
//
//	{ .cmd = { "/system/bin/chmod", "777", "/data/misc/net/rt_tables", NULL } },
//	{ .cmd = { "/system/bin/echo", "\"1001 eth0\"", ">>", "/data/misc/net/rt_tables", NULL } },
//	{ .cmd = { "/system/bin/ip", "route", "add", "223.255.255.0/30", "dev", "eth0", "table", "1001", NULL } },
//	{ .cmd = { "/system/bin/ip", "rule", "add", "from", "all", "table", "eth0", "pref", "5998", NULL } },
//
//	{ .cmd = { "/system/bin/echo", "\"1002 eth1\"", ">>", "/data/misc/net/rt_tables", NULL } },
//	{ .cmd = { "/system/bin/ip", "route", "add", "192.168.0.0/24", "dev", "br0.100", "table", "1002", NULL } },
//	{ .cmd = { "/system/bin/ip", "route", "add", "192.168.1.0/24", "dev", "br0.101", "table", "1002", NULL } },
//	{ .cmd = { "/system/bin/ip", "route", "add", "192.168.2.0/24", "dev", "br0.102", "table", "1002", NULL } },
//	{ .cmd = { "/system/bin/ip", "route", "add", "192.168.3.0/24", "dev", "br0.103", "table", "1002", NULL } },
//	{ .cmd = { "/system/bin/ip", "route", "add", "192.168.4.0/24", "dev", "br0.104", "table", "1002", NULL } },
//
//	{ .cmd = { "/system/bin/ip", "route", "add", "192.168.10.0/24", "dev", "br0.110", "table", "1002", NULL } },
//
//	{ .cmd = { "/system/bin/ip", "route", "add", "239.0.0.0/24", "dev", "br0.100", "table", "1002", NULL } },
//	{ .cmd = { "/system/bin/ip", "route", "add", "239.0.3.0/24", "dev", "br0.103", "table", "1002", NULL } },
//	{ .cmd = { "/system/bin/ip", "route", "add", "239.0.4.0/24", "dev", "br0.104", "table", "1002", NULL } },
//	{ .cmd = { "/system/bin/ip", "rule", "add", "from", "all", "table", "br0", "pref", "6998", NULL } },
//
//	{ .cmd = { "/system/bin/echo", "\"1004 defa_netmask\"", ">>", "/data/misc/net/rt_tables", NULL } },
//	{ .cmd = { "/system/bin/ip", "route", "add", "default", "via", "192.168.1.1", "dev", "br0.101", "table", "1004", NULL } },
//	{ .cmd = { "/system/bin/ip", "rule", "add", "from", "all", "table", "defa_netmask", "pref", "28888", NULL } },
//
//	{ .cmd = { "/system/bin/ndc", "resolver", "setnetdns", "br0.101", "\"\"", "218.4.4.4", NULL } },
//
//	{ .cmd = { "/system/bin/iptables", "-t", "nat", "-A", "POSTROUTING", "-d", "192.168.0.0/24", "-o", "br0.100", "-j", "MASQUERADE", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-t", "nat", "-A", "POSTROUTING", "-d", "192.168.1.0/24", "-o", "br0.101", "-j", "MASQUERADE", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-t", "nat", "-A", "POSTROUTING", "-d", "192.168.2.0/24", "-o", "br0.102", "-j", "MASQUERADE", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-t", "nat", "-A", "POSTROUTING", "-d", "192.168.3.0/24", "-o", "br0.103", "-j", "MASQUERADE", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-t", "nat", "-A", "POSTROUTING", "-d", "192.168.4.0/24", "-o", "br0.104", "-j", "MASQUERADE", NULL } },
//
//	{ .cmd = { "/system/bin/iptables", "-A", "oem_fwd", "-s", "192.168.30.0/24", "-o", "br0.100", "-j", "ACCEPT", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-A", "oem_fwd", "-s", "192.168.30.0/24", "-o", "br0.101", "-j", "ACCEPT", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-A", "oem_fwd", "-s", "192.168.30.0/24", "-o", "br0.102", "-j", "ACCEPT", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-A", "oem_fwd", "-s", "192.168.30.0/24", "-o", "br0.103", "-j", "ACCEPT", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-A", "oem_fwd", "-s", "192.168.30.0/24", "-o", "br0.104", "-j", "ACCEPT", NULL } },
//
//	{ .cmd = { "/system/bin/echo", "\"1005 usb0\"", ">>", "/data/misc/net/rt_tables", NULL } },
//
//	{ .cmd = { "/system/bin/ifconfig", "usb0", "192.168.30.1", "netmask", "255.255.255.0", "up", NULL } },
//
//	{ .cmd = { "/system/bin/ip", "route", "add", "192.168.30.0/24", "dev", "usb0", "table", "1005", NULL } },
//	{ .cmd = { "/system/bin/ip", "route", "add", "239.0.30.0/24", "dev", "usb0", "table", "1005", NULL } },
//	{ .cmd = { "/system/bin/ip", "rule", "add", "from", "all", "table", "usb0", "pref", "7000", NULL } },
//
//	{ .cmd = { "/system/bin/iptables", "-t", "nat", "-A", "POSTROUTING", "-d", "239.0.30.0/24", "-o", "usb0", "-j", "MASQUERADE", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-t", "nat", "-A", "POSTROUTING", "-d", "192.168.30.0/24", "-o", "usb0", "-j", "MASQUERADE", NULL } },
//
//	{ .cmd = { "/system/bin/iptables", "-A", "oem_fwd", "-d", "192.168.30.0/24", "-o", "usb0", "-j", "ACCEPT", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-A", "oem_fwd", "-d", "239.0.30.0/24", "-o", "usb0", "-j", "ACCEPT", NULL } },
//
//	{ .cmd = { "/system/bin/iptables", "-t", "nat", "-A", "POSTROUTING", "-s", "192.168.100.0/24", "-o", "br0.110", "-j", "MASQUERADE", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-t", "nat", "-A", "POSTROUTING", "-d", "192.168.100.0/24", "-o", "wlan0", "-j", "MASQUERADE", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-A", "oem_fwd", "-s", "192.168.100.0/24", "-o", "br0.110", "-j ACCEPT", NULL } },
//	{ .cmd = { "/system/bin/iptables", "-A", "oem_fwd", "-d", "192.168.100.0/24", "-o", "wlan0", "-j", "ACCEPT", NULL } },
//
//	{ .cmd = { "/system/bin/echo", "1", ">", "/proc/sys/net/ipv4/ip_forward", NULL } },
};



#endif /* __SHELL_DRIVER_H__ */