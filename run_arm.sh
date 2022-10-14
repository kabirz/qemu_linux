#!/bin/sh

if [ ! -d out ]; then
	mkdir out
fi
if [ ! -e out/rootfs_arm.ext4 ];then
	tar xf images/arm.tar.xz -C out
fi
BUILD_DIR=linux/out_vexpress
qemu-system-arm \
	-M vexpress-a9 \
	-smp 1 \
	-m 256 \
	-kernel ${BUILD_DIR}/arch/arm/boot/zImage \
	-dtb ${BUILD_DIR}/arch/arm/boot/dts/vexpress-v2p-ca9.dtb \
	-drive file=out/rootfs_arm.ext4,if=sd,format=raw \
	-append "console=ttyAMA0,115200 rootwait root=/dev/mmcblk0"  \
	-net nic,model=lan9118 \
	-net user  \
	-nographic
