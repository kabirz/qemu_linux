#!/bin/bash

rm out/rootfs.ext4 -fr
dd if=/dev/zero of=out/rootfs.ext4 bs=1K count=100K > /dev/null 2>&1

parted -s out/rootfs.ext4 mklabel msdos
parted -s out/rootfs.ext4 mkpart primary ext4 1M 100%

mkfs.ext4 -L ROOTFS out/rootfs.ext4  << EOF
y
EOF

mkdir mnt
sudo mount out/rootfs.ext4 mnt
tar xf rootfs.tar.xz -C /tmp
sudo cp /tmp/rootfs/* mnt -frd
sudo umount mnt

rm mnt -fr
rm /tmp/rootfs -fr
