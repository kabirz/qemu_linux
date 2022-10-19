obj-m := timer1.o timer2.o hrtimer1.o hrtimer2.o
all:
	@make -j8 --no-print-directory -C $(BUILD_DIR) M=`pwd` ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules
	@make -j8 --no-print-directory -C $(BUILD_DIR) M=`pwd` ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules_install INSTALL_MOD_PATH=_modules
clean:
	@make -j8 --no-print-directory -C $(BUILD_DIR) M=`pwd` clean
