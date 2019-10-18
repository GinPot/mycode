#! /bin/sh

echo "QNX Model: " $QNX_MODEL

to_bmetrics()
{
        print ${1+"${@}"} > /dev/bmetrics
}

log_launch()
{
        to_bmetrics bootmarker ${1}: launching
}

log_ready()
{
        to_bmetrics bootmarker ${1} ready
}

fde_setup () {
    log_launch "fde_ctl"
    #modify /dev/disk/uda0 permissions
    #chmod 666 /dev/disk/uda0
    on $FDE_CTL_ON_ARGS $FDE_CTL_BINARY $FDE_CTL_ARGS
    rc=$?

    case $rc in
        1)  #First run or user_pass has changed or 'ssd' got busted, seed userdata. 
            echo "$DEV_PATH/userdata's ICE key provisioned, seed the /var directory."
            seed_userdata;
            ;;
        0)  echo "Successfully set the FDE key for $DEV_PATH/userdata"
            ;;
        *)  echo "Failed to set input key for $DEV_PATH/userdata, rc=${rc}"
            return 1;
            ;;
    esac
}

seed_userdata() {
    echo "All data within /var would be lost"
    mkqnx6fs -i 32768 -b 32768 $DEV_PATH/userdata -q -T media
    if [ $? -ne 0 ] ; then
        echo " failed to format userdata"
        return 1
    fi

    mount -t qnx6 $DEV_PATH/userdata /var
    if [ $? -ne 0 ] ; then
        echo " failed to mount userdata"
        return 1
    fi

    echo "Seed initial directories in /var"
    echo "Creating /var/log"
    mkdir -p -m 0766 /var/log
    mkdir -p -m 0766 /var/run
    for adir in /var/ssh         /var/chroot         /var/chroot/sshd         /var/data         /var/pps_persist;
    do
        echo "Creating ${adir}"
        mkdir -p -m 0700 ${adir}
    done
	
	if [ $SECPOL_ENABLE -eq 1 ]; then	
        #add permission for secure build
        setfacl -m user:qseecom_daemon:w /persist
        setfacl -m group:qseecom_daemon:w /persist
        #TODO: check why setfacl doesn't work here
        #setfacl -m user:qseecom_daemon:rw /var/data
        #setfacl -m group:qseecom_daemon:rw /var/data
        #setfacl -m other::rw /var/data
        chmod 0766 /var/data
        chown 40:40 /var/pps_persist
	fi

    #All done, sync it up and unmount it.
    sync && umount -f /var
    return 0
}

mount_userdata(){
    DEV_PATH="/dev/disk";

    if [ -z $ENABLE_FDE ];  then
        echo "FDE for userdata is disabled";
    elif [ $ENABLE_FDE -eq 0 ]; then
        echo "FDE for userdata is disabled explicitly";
    else 
        fde_setup
        if [ $? -ne 0 ] ; then
          return 1
        fi
    fi

    if [[ ! -e /persist/.var_formatted ]]; then
        echo "First run ever, seed the /var directory."
        seed_userdata;
        touch /persist/.var_formatted && sync;
    fi

    mount -t qnx6 $DEV_PATH/userdata /var
    if [ $? -ne 0 ] ; then
        echo "failed to mount userdata";

        echo "checking $DEV_PATH/userdata";
        chkqnx6fs -vvvv $DEV_PATH/userdata;

        echo "checking super-block 0 $DEV_PATH/userdata";
        chkqnx6fs -S 0 $DEV_PATH/userdata;

        echo "checking super-block 1 $DEV_PATH/userdata";
        chkqnx6fs -S 1 $DEV_PATH/userdata;

        return 1
    fi

    #Symlink /data 
    qln -sfP /var/data /data
}

common_early () {
#Add this here , so that we can easily enable slog on boot whe needed
#rm -f /var/third_lastrun.txt
#mv -f /var/slog_second_lastrun.txt /var/slog_third_lastrun.txt
#mv -f /var/slog_lastrun.txt /var/slog_second_lastrun.txt
#mv -f /var/slog_currentrun.txt /var/slog_lastrun.txt
#on -d slog2info -t -w > /var/slog_currentrun.txt

    on $RANDOM_ON_ARGS $RANDOM_BINARY $RANDOM_ARGS 

}

common_late () {
    # Tuning for network throughput performance
    # IPv4
    sysctl -q -w net.inet.udp.sendspace=92160 net.inet.udp.recvspace=166400
    sysctl -q -w net.inet.tcp.sendspace=90112 net.inet.tcp.recvspace=1000000
	sysctl -w qnx.net.inet.tcp.qnx_delack=1
	sysctl -w net.inet.tcp.ack_on_push=1

    # ARP
    sysctl -w net.inet.arp.keep=1800
    # IPv6
    sysctl -q -w net.inet6.udp6.sendspace=92160 net.inet6.udp6.recvspace=166400
    sysctl -q -w net.inet6.tcp6.sendspace=90112 net.inet6.tcp6.recvspace=1000000
    sysctl -q -w kern.sbmax=8388608 kern.mbuf.nmbclusters=81920

    #enable if requested gdb
    if [[ -e /var/enable_gdb ]]; then
        pdebug 8888
        echo "Process level ( pdebug ) enabled"
        echo "Run 'qnx attach <ip_addr>:8888' to connect remotely via gdb'"
    else 
        echo "pdebug/gdbserver not enabled by default . touch /var/enable_gdb and reset device to enable gdb debugging"
    fi

    ##  start Analyzer; user responsible for creating/deleting flag
    if [ -e /etc/enable_analyzer ]
    then
        #echo "Info startup.sh: Analyzer enabled by default (/etc/enable_analyzer)"
        # Start Analyzer
        /scripts/analyzer.sh start
    else 
        echo "Analyzer not enabled. touch /etc/enable_analyzer and reset device to enable Analyzer debugging"
	##  start Analyzer External; user responsible for creating/deleting flag
	if [ -e /etc/enable_analyzer_external ]
	then
 	    echo "Info startup.sh: Analyzer External enabled by default (/etc/enable_analyzer_external)"
	    # Start Analyzer
	    /scripts/analyzer.sh start tpm
	else 
	    echo "Analyzer External not enabled. touch /etc/enable_analyzer_external and reset device to enable Analyzer External debugging"
	fi		
    fi	
}

common_onetime () {
    #Enable SSH 
    if [[ ! -e /var/ssh/ssh_host_dsa_key ]] then
        echo "Generate dsa key for ssh only on first boot"
        ssh-keygen -t dsa -b 1024 -f /var/ssh/ssh_host_dsa_key -N ''
        echo "Generate rsa key for ssh only on first boot"
        ssh-keygen -t rsa -b 1024 -f /var/ssh/ssh_host_rsa_key -N ''
    fi
}

common_netdbgservices () {
    
    on $SYSLOGD_ON_ARGS $SYSLOGD_BINARY $SYSLOGD_ARGS

    inetd /etc/inetd.conf  > /dev/null &
    #TODO ; find out why does not it work with - on  and args as  syslogd above
    devc-pty -n 32 &
    qconn &
}

common_pps () {
    log_launch "pps"
    #TODO pps progmgr_ability fail, need fix
    if [[ -e /etc/pps_acl.conf ]] then
        if [ $SECPOL_ENABLE -eq 1 ]; then
            on -T pps_t -d /bin/pps -A /etc/pps_acl.conf -m /var/pps -p /var/pps_persist -t 100 -U 40:40
        else
            pps -A /etc/pps_acl.conf -m /var/pps -p /var/pps_persist -t 100
        fi
    else
        if [ $SECPOL_ENABLE -eq 1 ]; then
            on -T pps_t -d /bin/pps -m /var/pps -p /var/pps_persist -t 100 -U 40:40
        else
        	pps -m /var/pps -p /var/pps_persist -t 100
        fi
    fi

    if [ $SECPOL_ENABLE -eq 1 ]; then
		waitfor /var/pps
		chown 40:40 /var/pps
    fi
}

common_hid () {
    log_launch "io-hid"
    io-hid -dusb
}

common_create_if() {
# $1 == interface name

    on $IFCONFIG_ON_ARGS $IFCONFIG_BINARY $1 create
    on $IFUP_ON_ARGS $IFUP_BINARY -p $1 
    on $IFCONFIG_ON_ARGS $IFCONFIG_BINARY $1 up
}

common_add_to_bridge () {
# $1 == bridge name
# $2 == interface name
    brconfig $1 add $2 up
}

common_create_peer_if () {
# $1 == 'system' name from qvm config file
# $2 == host side pathspace's basename.
# $3 == interface name
# $4 == mac address
# $5 == interface name
    if [[ -z $5 ]]; then
	INTERFACE=vp
    else
	INTERFACE=$5
    fi
    mount -T io-pkt -o peer=/dev/qvm/$1/$2,bind=/dev/vdevpeers/$3,mac=$4,name=$INTERFACE /lib/dll/devnp-vdevpeer.so
    on $IFUP_ON_ARGS $IFUP_BINARY -p $3
    on $IFCONFIG_ON_ARGS $IFCONFIG_BINARY $3 up
}

launch_ssr_service() {
	waitfor /var/pps
    on $SSR_SERVICE_ON_ARGS $SSR_SERVICE_BINARY $SSR_SERVICE_ARGS
}

launch_vm_ssr_be() {
	on $VM_SSR_BE_ON_ARGS $VM_SSR_BE_BINARY $VM_SSR_BE_ARGS
}

launch_heartbeat() {
	if [[ -e /var/heartbeat ]] then
		echo "Launching Heartbeat ...."
		heartbeat -f -m sys=50.0,start=40.0 -c sys=50.0 &
	fi
}

launch_wcnss_be() {
    log_launch "wcnss_be"
    #WCNSS_BE_ARGS [ verbosity: info = 5 ]
    on $WCNSS_BE_ON_ARGS $WCNSS_BE_BINARY $WCNSS_BE_ARGS
}

get_mac_from_syspage() {
# $1 == os indicator, generate different mac addr for different guest
# $2 == interface number
    MAC1=`pidin syspage | sed -n -e "/(nicaddr)/ {n;p;n;n;n;n}" | sed -n -e "s/^.\{17\}//p" | sed -n -e "s/.\{12\}$//p" | sed -n -e "s/ /:/gp"`
    MAC2=`pidin syspage | sed -n -e "/(nicaddr)/ {n;n;n;p;n;n}" | sed -n -e "s/^.\{17\}//p" | sed -n -e "s/.\{12\}$//p" | sed -n -e "s/ /:/gp"`
    MAC3=`pidin syspage | sed -n -e "/(nicaddr)/ {n;n;n;n;n;p}" | sed -n -e "s/^.\{17\}//p" | sed -n -e "s/.\{12\}$//p" | sed -n -e "s/ /:/gp"`
    case $1 in
        vm1)
            MACALL=$MAC2:$MAC1:$MAC3
        ;;
        vm2)
            MACALL=$MAC3:$MAC2:$MAC1
        ;;
        vm3)
            MACALL=$MAC3:$MAC1:$MAC2
        ;;
        *)
            MACALL=$MAC1:$MAC2:$MAC3
        ;;
    esac
    case $2 in
        1)
            MAC=`echo $MACALL | sed -n -e "s/^.\{3\}//gp" | sed -n -e "s/.\{18\}$//gp"`
        ;;
        2)
            MAC=`echo $MACALL | sed -n -e "s/^.\{6\}//gp" | sed -n -e "s/.\{15\}$//gp"`
        ;;
        3)
            MAC=`echo $MACALL | sed -n -e "s/^.\{9\}//gp" | sed -n -e "s/.\{12\}$//gp"`
        ;;
        4)
            MAC=`echo $MACALL | sed -n -e "s/^.\{12\}//gp" | sed -n -e "s/.\{9\}$//gp"`
        ;;
        5)
            MAC=`echo $MACALL | sed -n -e "s/^.\{15\}//gp" | sed -n -e "s/.\{6\}$//gp"`
        ;;
        6)
            MAC=`echo $MACALL | sed -n -e "s/^.\{18\}//gp" | sed -n -e "s/.\{3\}$//gp"`
        ;;
    esac
    MAC=02:$MAC
    echo $MAC
}

dhcp_client_set()
{
# $1 == cookie file to trigger DHCP e.g. /var/dhcp_ntn
# $2 == interface name
    if [ -e $1 ]; then
        export PATH_DHCLIENT_DB=/var/dhclient.leases
        export PATH_DHCLIENT_SCRIPT=/sbin/dhclient-script
        export PATH_DHCLIENT_CONF=/etc/dhclient.conf
        echo "($2): $1 was found -> System is configured to use DHCP"
        dhclient -m $2 &
    fi

}
km_loader () {
      waitfor /dev/qseecom
      log_launch "km_loader"
      on $KM_LOADER_DAEMON_ON_ARGS $KM_LOADER_DAEMON_BINARY $KM_LOADER_DAEMON_ARGS        
}

qseecom () {

    log_launch "qseecom_service"
    on $QSEECOM_SERVICE_ON_ARGS $QSEECOM_SERVICE_BINARY $QSEECOM_SERVICE_ARGS &

    #modify rpmb device permissions
	if [[ "$uname_m" == *UFS* ]]; then   
		chmod 666 /dev/disk/uda7
    else 
	    chmod 666 /dev/disk/rpmb0
	fi
    #TODO: to be removed once all qseecom changes are ported
    chmod 666 /dev/disk/uda*
    log_launch "qseecom_daemon"
    on $QSEECOM_DAEMON_ON_ARGS $QSEECOM_DAEMON_BINARY $QSEECOM_DAEMON_ARGS
}

io-audio-deva () {
    DEVA_ARGS="skip_device_disable=0,bmetrics_level=medium,log_level=high,platform_id=demo,mib_cgms=0"
    waitfor /dev/audio_service
    log_launch "io-audio"
    on $IOAUDIO_ON_ARGS $IOAUDIO_BINARY -d qc ${DEVA_ARGS}
}

thermal () {
    #FIXME  Move enable_dplmp to QCORE once thermal is verified
#    log_launch "enable_dplmp"
#    echo 'dlexec "" "enable_dplmp" "0" GLOBALSYMS ' > /dev/qcore
    waitfor /dev/dplmp

    log_launch "io-service"
    on $IOSERVICE_ON_ARGS $IOSERVICE_BINARY $IOSERVICE_ARGS

}

modem() {
    log_launch "mis"
    if [ $SECPOL_ENABLE -eq 1 ]; then
        # TODO Remove this line once block devices create with proper permission
        chmod 666 /dev/disk/uda*
    fi
    on $MODEM_ON_ARGS $MODEM_BINARY $MODEM_ARGS &
    
    log_launch "mode_reset"
    on $MODE_RESET_ON_ARGS $MODE_RESET_BINARY $MODE_RESET_ARGS &
}

# $1 - Host IP address
# $2 - DIAG BE Preferred Client IP address
qcdiag () {
    log_launch "qcdiag"    
    ##  start DIAG Services; user responsible for creating/deleting flag
    if [ -e /etc/enable_diag_services ]
    then
        echo "Info startup.sh: DIAG Services enabled by default (/etc/enable_diag_services)"	
        if [ -z "$1" ]; then
            DIAG_SERVICE_ARGS="--serverip=192.168.0.10 --log_level=7" 
        else
            DIAG_SERVICE_ARGS="--serverip=$1 --log_level=7"
        fi
        #echo $DIAG_SERVICE_ARGS 
        on $DIAG_SERVICE_ON_ARGS $DIAG_SERVICE_BINARY $DIAG_SERVICE_ARGS &

        if [ -z "$2" ]; then
            DIAG_LSM_SERVERSOCKET_ARGS="-i 192.168.0.2" 
        else
            DIAG_LSM_SERVERSOCKET_ARGS="-i $2"
        fi
        
        log_launch "diag_lsm_serversocket"
        on $DIAG_LSM_SERVERSOCKET_ON_ARGS $DIAG_LSM_SERVERSOCKET_BINARY $DIAG_LSM_SERVERSOCKET_ARGS&
        
    else 
        echo "DIAG Services not enabled. touch /etc/enable_diag_services and reset device to enable DIAG Service"
    fi
}

sdcard () {
    log_launch "devb-sdcard" 
    if [[ "$uname_m" == *Mojave-v1* ]]; then
        ${SD_LAUNCH_ON_ARGS} /bin/devb-sdmmc-msm8996 blk cache=10M,noatime,ra=128k:128k,automount=@/etc/sdcard.mnt.rules disk name=sda cam quiet,cache,pnp sdmmc verbose=0,bs=dname=compat sdio idx=1,bs=cd=95
    else
        ${SD_LAUNCH_ON_ARGS} /bin/devb-sdmmc-msm8996 blk cache=10M,noatime,ra=128k:128k,automount=@/etc/sdcard.mnt.rules disk name=sda cam quiet,cache,pnp sdmmc verbose=0,bs=dname=compat sdio idx=1,bs=cd=38
    fi
}

bt-ser() {
    #uart instance for BT
    devc-sermsm8x60 -u5 -S -E -f -b115200 0x07570000
}

csd-uart() {
		#uart instance for CSD /dev/ser2
		devc-sermsm8x60 -u2 -e -b115200 -F -d 0x07574000
		#gpio 26 no pull only for PMA E3 board
		msmgpiotool set-pull 26 0
}

powerman () {
    echo 'dlexec "" "idleHookCb_register" "0" GLOBALSYMS ' > /dev/qcore
}

usb-devcfg() {
    echo 'dlexec "devcfg_usb.so" "usb_devcfg_init" "" GLOBALSYMS ' > /dev/qcore
}

camera_avin_server () {
    log_launch "avin_server"
    if [ $SECPOL_ENABLE -eq 1 ];then
        on -T avin_server_t -d avin_server -U 75:75,33,21,62 &
    else
       avin_server  &
    fi
}

camera_ais_server () {
    log_launch "camera_server"
    if [ $SECPOL_ENABLE -eq 1 ];then
	on -T ais_server_t -u 62:62,29,21,75 -d ais_server &
    else
       ais_server  &
    fi
}

prng() {
    log_launch "prng_service"
    on $PRNG_SERVICE_ON_ARGS $PRNG_SERVICE_BINARY $PRNG_SERVICE_ARGS &
}

gvm-cleanup() {
    log_launch "gvm_cleanup"
    on $GVM_RESTART_CLEANUP_ON_ARGS $GVM_RESTART_CLEANUP_BINARY $GVM_RESTART_CLEANUP_ARGS &
}

runusb2(){
    USB20_PORT=0x7600000
    USB20_IRQ=170

    IOUSB_DEVICE_OPTS=$IO_USB_DEVICE_verbosity
    DEVU_DEVICE_OPTS=ioport=$USB20_PORT,irq=$USB20_IRQ,verbose=$DEVU_DEVICE_verbosity

    IOUSB_HOST_OPTS=$IO_USB_HOST_verbosity
    DEVU_HOST_OPTS=ioport=$USB20_PORT,irq=$USB20_IRQ,verbose=$DEVU_HOST_verbosity

    if [ ! -z ${USB_TYPED_MEM} ] ; 
    then
        IOUSB_HOST_OPTS="${IOUSB_HOST_OPTS} -t memory=${USB_TYPED_MEM}"
        IOUSB_DEVICE_OPTS="${IOUSB_DEVICE_OPTS} -t memory=${USB_TYPED_MEM}"

        DEVU_DEVICE_OPTS="${DEVU_DEVICE_OPTS},memory=${USB_TYPED_MEM}"
        DEVU_HOST_OPTS="${DEVU_HOST_OPTS},memory=${USB_TYPED_MEM}"
    fi

    log_launch "USB2"

    if [[ -e /var/rndis1 ]]; then
        on $IO_USB_ON_ARGS $IO_USB_BINARY $IOUSB_DEVICE_OPTS -dusbrndis-msm8996-dwcotg3 $DEVU_DEVICE_OPTS
        waitfor /dev/usb/io-usb-otg  
        chmod 666 /dev/usb/io-usb-otg
        on $USB_POWER_DOWN_ON_ARGS $USB_DEVICE_POWER_DOWN_BINARY -p /dev/usb/io-usb-otg

        mount -Tio-pkt -o protocol=rndis,mac=000200000002,usbdnet_mac=000200000001,verbose=$DEVNP_DNET_verbosity,path="/dev/usb/io-usb-otg" devnp-usbdnet.so
        on $IFUP_ON_ARGS $IFUP_BINARY -p rndis0
        on $IFCONFIG_ON_ARGS $IFCONFIG_BINARY rndis0 192.168.0.4/24 
    elif [[ -e /var/serusb1 ]]; then
        on $IO_USB_ON_ARGS $IO_USB_BINARY $IOUSB_DEVICE_OPTS -dusbser-msm8996-dwcotg3 $DEVU_DEVICE_OPTS
        waitfor /dev/usb/io-usb-otg 
        chmod 664 /dev/usb/io-usb-otg
        on $USB_POWER_DOWN_ON_ARGS $USB_DEVICE_POWER_DOWN_BINARY -p /dev/usb/io-usb-otg
        devc-serusb_dcd -S -E -f -b 115200 -d path="/dev/usb/io-usb-otg"
    else
        on $IO_USB_ON_ARGS $IO_USB_BINARY $IOUSB_HOST_OPTS -dxhci $DEVU_HOST_OPTS
        waitfor /dev/usb/io-usb-otg 
        chmod 664 /dev/usb/io-usb-otg
        on $USB_POWER_DOWN_ON_ARGS $USB_HOST_POWER_DOWN_BINARY -p /dev/usb/io-usb-otg
    fi
    waitfor /var/pps
    on $USBLAUNCHER_OTG_ON_ARGS $USBLAUNCHER_OTG_BINARY -S 1 -c /etc/usblauncher/rules.lua -M /etc/usblauncher/rules.mnt -m /var/pps -n /dev/usb/io-usb-otg -E -e -0
}

runusb3(){
    USB30_PORT=0x6A00000
    USB30_IRQ=163

    IOUSB_DEVICE_OPTS="$IO_USB_DEVICE_verbosity -n  /dev/otg/io-usb-otg"
    DEVU_DEVICE_OPTS=ioport=$USB30_PORT,irq=$USB30_IRQ,verbose=$DEVU_DEVICE_verbosity

    IOUSB_HOST_OPTS="$IO_USB_HOST_verbosity -n /dev/otg/io-usb-otg"
    DEVU_HOST_OPTS=ioport=$USB30_PORT,irq=$USB30_IRQ,verbose=$DEVU_HOST_verbosity

    if [ ! -z ${USB_TYPED_MEM} ] ; 
    then
        IOUSB_HOST_OPTS="${IOUSB_HOST_OPTS} -t memory=${USB_TYPED_MEM}"
        IOUSB_DEVICE_OPTS="${IOUSB_DEVICE_OPTS} -t memory=${USB_TYPED_MEM}"

        DEVU_DEVICE_OPTS="${DEVU_DEVICE_OPTS},memory=${USB_TYPED_MEM}"
        DEVU_HOST_OPTS="${DEVU_HOST_OPTS},memory=${USB_TYPED_MEM}"
    fi

    log_launch "USB3"
        
    if [[ -e /var/rndis ]]; then
        on $IO_USB_ON_ARGS $IO_USB_BINARY $IO_USB_DCD_verbosity $IOUSB_DEVICE_OPTS  -dusbrndis-msm8996-dwcotg3 $DEVU_DEVICE_OPTS 

        waitfor /dev/otg/io-usb-otg 
        chmod 666 /dev/otg/io-usb-otg
        on $USB_POWER_DOWN_ON_ARGS $USB_DEVICE_POWER_DOWN_BINARY -p /dev/otg/io-usb-otg

        mount -Tio-pkt -o protocol=rndis,mac=000200000002,usbdnet_mac=000200000001,verbose=$DEVNP_DNET_verbosity,path="/dev/otg/io-usb-otg" devnp-usbdnet.so
        on $IFUP_ON_ARGS $IFUP_BINARY -p rndis0
        on $IFCONFIG_ON_ARGS $IFCONFIG_BINARY rndis0 192.168.0.4/24 
    elif [[ -e /var/serusb ]]; then
        on $IO_USB_ON_ARGS $IO_USB_BINARY $IO_USB_DCD_verbosity $IOUSB_DEVICE_OPTS -dusbser-msm8996-dwcotg3 $DEVU_DEVICE_OPTS
        waitfor /dev/otg/io-usb-otg 
            chmod 664 /dev/otg/io-usb-otg
        on $USB_POWER_DOWN_ON_ARGS $USB_DEVICE_POWER_DOWN_BINARY -p /dev/otg/io-usb-otg
        devc-serusb_dcd -S -E -f -b 115200 -d path="/dev/otg/io-usb-otg"
    else
        on $IO_USB_ON_ARGS $IO_USB_BINARY $IO_USB_verbosity $IOUSB_HOST_OPTS -dxhci $DEVU_HOST_OPTS
 
        waitfor /dev/otg/io-usb-otg
        chmod 664 /dev/otg/io-usb-otg
        on $USB_POWER_DOWN_ON_ARGS $USB_HOST_POWER_DOWN_BINARY -p /dev/otg/io-usb-otg
    fi

    waitfor /var/pps
    on $USBLAUNCHER_OTG_ON_ARGS $USBLAUNCHER_OTG_BINARY -S 0 -c /etc/usblauncher/otg.lua -M /etc/usblauncher/rules.mnt -m /var/pps -n /dev/otg/io-usb-otg -E -e -0
}

usb() {
    log_launch "io-usb"
    if [ -f /var/usbhost_verbose ];
    then
        IO_USB_HOST_verbosity="-vvvvvv"
        DEVU_HOST_verbosity=10
    else
        IO_USB_HOST_verbosity=""
        DEVU_HOST_verbosity=2
    fi

    if [ -f /var/usbdevice_verbose ]
    then
        IO_USB_DEVICE_verbosity="-vvvvvv"
        DEVU_DEVICE_verbosity=10
        DEVNP_DNET_verbosity=10
    else
        IO_USB_DEVICE_verbosity=""
        DEVU_DEVICE_verbosity=2
        DEVNP_DNET_verbosity=2
    fi

    if [ ! -z ${USB2_ENABLE} ] && [ ${USB2_ENABLE} -eq 1 ]
    then
        runusb2;
    fi

    if [ ! -z ${USB3_ENABLE} ] && [ ${USB3_ENABLE} -eq 1 ]
    then
        runusb3;
    fi

}

mm_backends () {
    log_launch "ais_be_server"
    if [ $SECPOL_ENABLE -eq 1 ];then
        on -T ais_be_server_t -u 63:63 -d ais_be_server &
    else
        ais_be_server  &
    fi
}


enwlan() {
    spmitool.sh write 0 0xc440 0x10
    spmitool.sh write 0 0xc440 0x11
}

USB2_ENABLE=1;

. /scripts/platform_variables.sh
uname_m=`uname -m`

dt_patch -i /vm/images/vplatform-lfv-msm8996-ivi-la.dtb -o /vm/images/agl_1.dtb -p label=pmem_shared_mem,replace=highmem.0 -p label=ion_system_mem,replace=highmem.1 -p label=ion_audio_mem,replace=gvm2_phys2
#dt_patch -i /vm/images/vplatform-lfv-msm8996-ivi-la-recovery.dtb -o /vm/images/agl_2.dtb -p label=pmem_shared_mem,replace=highmem.0 -p label=ion_system_mem,replace=highmem.1 -p label=ion_audio_mem,replace=gvm2_phys2

echo "Running APS"
aps modify -S limit_cpu_usage
aps create -b80  A
aps modify -m 85 A
aps create -b15  B
aps modify -m 90 B
log_launch "LA GVM"
mqueue &
hal_init
os_shm_init
lib_config_init
#spidbgr 9 0 0x2208 2000000 read 20
on -X aps=B lifecycle &


for vm in agl_1; do
	echo "vm=${vm} uart output copied to /dev/shmem/${vm}.txt";
	touch /dev/shmem/${vm}.txt;
done 
usb-devcfg;
on $VMM_SERVICE_ON_ARGS $VMM_SERVICE_BINARY $VMM_SERVICE_ARGS

#echo "Running APS"
#aps modify -S limit_cpu_usage
#aps create -b80  A
#aps modify -m 85 A
#aps create -b15  B
#aps modify -m 90 B
echo "vm=agl_2 uart output copied to /dev/shmem/agl_2.txt";
touch /dev/shmem/agl_2.txt
recovery_mode
if [ "$?" == "0" ] ; then
	echo "Calling LA recovery kernel"
	sleep 3
	echo control 1 3 > /dev/vmm
else
	echo "Running LA VM"
#	echo control 1 2 > /dev/vmm
fi

#start the vmm revovery client, APP GVM ID is 2, recovery GVM ID is 3
#recovery_mode -v -a 2 -r 3 &

common_early;
thermal;
qseecom;
io-audio-deva;
mount_userdata;
prng;
#We have mounted the last partition, drop root now.
devb_drop_root -d /dev/disk/uda0

common_pps;
/bin/netconfig.sh netconfig
usb;
common_onetime ;
common_netdbgservices ;
qcdiag 192.168.1.10 192.168.1.2;
camera_avin_server;

## For early camera, ais server started from ifs2
camera_ais_server;

mm_backends;
powerman;
km_loader;
io-hid -dusb
enusb > /dev/null
enwlan
launch_ssr_service;
launch_vm_ssr_be;
#add new uart for external CSD uart
csd-uart;
cfg_tpirq
# Add network top2
#ifconfig tap2 create
#brconfig bridge0 add tap2
#ifconfig tap2 192.168.0.4 up
# run dubs  
#export $(dbus-launch --config-file=/opt/session.conf)
#dbus-launch --config-file=/opt/session_TCP.conf

#mtouch -c /etc/system/config/mtouch_lilliput_1080p_SiS.conf &
#calib-touch -config-file=/persist/calib.lilliput.1080p

#mtouch -c /etc/system/config/mtouch_lilliput_720p_TPK.conf &
#mtouch -c /etc/system/config/mtouch_lilliput_720p_eGalax.conf &
#calib-touch -config-file=/persist/calib.lilliput.720p

common_late

#setfacl is needed as /dev/vdevpeers/* are created by io_pkt with mask of 600.
#Since qnx has not got rid of linux styele file permissions,qvm need access to these pathspaces for host-guest,guest-guest communication.
setfacl -m user:206:rw /dev/vdevpeers/vp0
setfacl -m user:206:rw /dev/vdevpeers/ntn_vp0

echo "Calling Cluster Application"
if [ -x /usr/autodemo/cluster/runCluster.sh ]; then
	echo "Cluster app exist "
	#. ./usr/autodemo/cluster/runCluster.sh
else
	echo "Cluster app does not exists"
fi

#drop qcore abilities
#mqueue &
#hal_init
#os_shm_init
#lib_config_init
#spidbgr 9 0 0x2208 2000000 read 20
#lifecycle &
dlt-daemon &
dlt-logstorage-ctrl -p /data/dlt/log/ -c 0 &
dlt-logstorage-ctrl -p /data/dlt/log/ -c 1 &
echo -n drop_ability \"\"prot_exec,map_fixed,prot_write_and_exec > /dev/qcore


echo "App Startup complete"