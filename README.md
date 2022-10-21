# qemu_linux
linux module for qemu
# copy kernel module from host
```bash
scp -r name@ip:GIT_ROOT/linux/out/_modules/lib/modules/linuxxxx/extra .
```

### kgdb
patch: patches/kdb_arm64.patch
config: patches/kdb_config.diff
1. [arm64 kgdb1](https://blog.csdn.net/qq_35712169/article/details/108217641) [arm64 kgdb2](https://zhuanlan.zhihu.com/p/197545583)

### earlycon
1. [earlycon](https://blog.csdn.net/ooonebook/article/details/52654191)


### [device tree](./doc/devicetree_cn.md)

#### 参考：
[Linux内核中的pinctrl子系统应用实例](https://blog.csdn.net/u012719256/article/details/76070339)  
[Linux - 使用V4L2（总结）](https://blog.csdn.net/weixin_43707799/article/details/107821189)  
[Linux V4L2之camera](https://www.cnblogs.com/vedic/p/10763838.html)  
[深入学习Linux摄像头（二）v4l2驱动框架](https://blog.csdn.net/weixin_42462202/article/details/99680969)
