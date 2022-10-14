#!/bin/sh

OUT=out_arm64
pushd linux
#config
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-  O=${OUT} defconfig
# build
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-  O=${OUT} -j8
# install moduels
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-  O=${OUT} modules_install INSTALL_MOD_PATH=_modules
