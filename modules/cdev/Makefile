obj-m := cdev1.o hello.o
SRC_DIR := ../../linux
all:
	@make -j8 --no-print-directory -C $(SRC_DIR)/out M=`pwd` ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules
	@make -j8 --no-print-directory -C $(SRC_DIR)/out M=`pwd` ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules_install INSTALL_MOD_PATH=_modules
clean:
	@make -j8 --no-print-directory -C $(SRC_DIR)/out M=`pwd` clean
