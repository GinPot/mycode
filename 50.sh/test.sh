echo "insmod DWC_ETH_QOS.ko";
insmod  DWC_ETH_QOS.ko;
echo "ifconfig eth2 up";
ifconfig eth2 up;

echo "brctl addbr br0"
brctl addbr br0;

echo "brctl  addif br0 eth1 eth2";
brctl  addif br0 eth1 eth2;
brctl  addif br0 eth1.100 eth2.100;
brctl  delif br0 eth1.100 eth2.100

echo "ifconfig br0 up";
ifconfig br0 up;

echo "ip addr del 192.168.7.1 dev eth1";
ip addr del 192.168.7.1 dev eth1;

toybox vconfig add eth1 100;
ifconfig eth1.100 192.168.0.15;

toybox vconfig add eth2 100;
ifconfig eth2.100 192.168.0.16;

brctl  addif br0 eth2.100;
brctl show br0


ifconfig  vlan100  create 
ifconfig vlan100 vlan 100 vlanif tap2
ifconfig vlan100 192.168.0.17
ping 192.168.0.17


ip route get 192.168.0.1

ifconfig eth1.110 