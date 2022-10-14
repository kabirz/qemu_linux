#!/bin/sh

OUT=out_vexpress
pushd linux
#config
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-  O=${OUT} multi_v7_defconfig
# build
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-  O=${OUT} -j8
# install moduels
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-  O=${OUT} modules_install INSTALL_MOD_PATH=_modules

# generate compile_commands.json
popd
# linux/scripts/clang-tools/gen_compile_commands.py -d linux/${OUT}
