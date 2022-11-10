#!/bin/bash
if [ ! -d out ]; then
	mkdir out
fi
if [ ! -e out/rootfs_arm64.ext4 ];then
	tar xf images/arm64.tar.xz -C out
fi

IP_HEADER="192.168.7"
TAP=`sudo tunctl -b -u $USER`
number=`echo $TAP | sed 's/tap//'`
n=`echo "number * 2 + 1" | bc`
sudo ip addr add $IP_HEADER.$n/32 broadcast $IP_HEADER.255 dev $TAP
sudo ip link set dev $TAP up
d=`echo "number * 2 + 2" | bc`
sudo ip route add to $IP_HEADER.$d dev $TAP

sudo iptables -A POSTROUTING -t nat -j MASQUERADE -s $IP_HEADER.$n/32
sudo iptables -A POSTROUTING -t nat -j MASQUERADE -s $IP_HEADER.$d/32
echo 1 |sudo tee /proc/sys/net/ipv4/ip_forward > /dev/null
echo 1 |sudo tee /proc/sys/net/ipv4/conf/$TAP/proxy_arp > /dev/null
sudo iptables -P FORWARD ACCEPT
IP="$IP_HEADER.$d::$IP_HEADER.$n:255.255.255.0::eth0:off:8.8.8.8"
KERNEL=linux/out_arm64/arch/arm64/boot/Image 
./generate_rootfs.sh
ROOTFS=out/rootfs.ext4
# ROOTFS=out/rootfs_arm64.ext4
qemu-system-aarch64 \
	-M virt \
	-cpu cortex-a53 \
	-nographic \
	-smp 4 \
	-kernel $KERNEL \
	-append "rootwait root=/dev/vda console=ttyAMA0 ip=$IP" \
	-device virtio-net-pci,netdev=net0,mac=52:54:00:12:34:02 \
	-netdev tap,id=net0,ifname=$TAP,script=no,downscript=no \
	-drive file=$ROOTFS,if=none,format=raw,id=hd0 \
	-device virtio-blk-device,drive=hd0


sudo tunctl -d $TAP
sudo iptables -D POSTROUTING -t nat -j MASQUERADE -s $IP_HEADER.$n/32
sudo iptables -D POSTROUTING -t nat -j MASQUERADE -s $IP_HEADER.$d/32

