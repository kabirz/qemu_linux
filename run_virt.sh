#!/bin/sh
if [ ! -d out ]; then
	mkdir out
fi
if [ ! -e out/rootfs_arm64.ext4 ];then
	tar xf images/arm64.tar.xz -C out
fi
qemu-system-aarch64 \
	-M virt \
	-cpu cortex-a53 \
	-nographic \
	-smp 1 \
	-kernel  linux/out/arch/arm64/boot/Image \
	-append "rootwait root=/dev/vda console=ttyAMA0" \
	-netdev user,id=eth0 \
	-device virtio-net-device,netdev=eth0 \
	-drive file=out/rootfs_arm64.ext4,if=none,format=raw,id=hd0 \
	-device virtio-blk-device,drive=hd0
