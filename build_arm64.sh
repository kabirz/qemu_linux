#!/bin/sh

OUT=out_arm64
pushd linux
#config
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- O=${OUT} defconfig
# build
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-  O=${OUT} -j`nproc`
# install moduels
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- O=${OUT} modules_install INSTALL_MOD_PATH=_modules

# generate compile_commands.json
popd
linux/scripts/clang-tools/gen_compile_commands.py -d linux/${OUT}
