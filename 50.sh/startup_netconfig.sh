#! /bin/sh

if [ -e /pss1/ether_gvm ]; then
	echo "Lodaing ethernet driver on QNX."
	
	io-pkt-v6-hc -v -p tcpip stacksize=8192,pagesize=65536,pkt_cache=1024,mbuf_cache=1024,pkt_typed_mem=/ram/dma -ppf-v6 -dvdevpeer peer=/dev/qvm/agl_1/agl_to_host,bind=/dev/vdevpeers/vp0,mac=a0b0c0d0e0f0,name=vp -dvdevpeer peer=/dev/qvm/agl_1/agl_to_host_eth1,bind=/dev/vdevpeers/ntn_vp0,mac=a0b0c0d0e5ff,name=ntn_vp -dntn-eth fw=/lib64/firmware/ntn_firmware.bin -U 37:37,21,35,77,87,46,40
	pipe &
	/bin/netconfig.sh netconfig-early
else
	echo "Lodaing ethernet driver on GVM."

	io-pkt-v6-hc -v -p tcpip stacksize=8192,pagesize=65536,pkt_cache=1024,mbuf_cache=1024,pkt_typed_mem=/ram/dma -ppf-v6 -dvdevpeer peer=/dev/qvm/agl_1/agl_to_host,bind=/dev/vdevpeers/vp0,mac=a0b0c0d0e0f0,name=vp -dvdevpeer peer=/dev/qvm/agl_1/agl_to_host_eth1,bind=/dev/vdevpeers/ntn_vp0,mac=a0b0c0d0e5ff,name=ntn_vp -U 37:37,21,35,77,87,46,40
	pipe &
	/bin/netconfig.sh netconfig-early
fi

GPIO_NUMBS=102

if [ "$(msmgpiotool read ${GPIO_NUMBS})" = "GPIO ${GPIO_NUMBS} is Low" ];then
	echo low
else
	echo high
fi

