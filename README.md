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
### [softirq tasklet](./doc/softirq_cn.md)  
### [rwsem-读写信号量](./doc/rwsem_cn.md)

#### 参考：
[Linux内核中的pinctrl子系统应用实例](https://blog.csdn.net/u012719256/article/details/76070339)  
[一文让你读懂Linux五大模块内核源码，内核整体架构设计（超详细）](https://zhuanlan.zhihu.com/p/474337723)  
[linux系统调度之时间](https://blog.csdn.net/eleven_xiy/article/details/71175347)  

[mtk linux](https://gitlab.com/mediatek/aiot/bsp/linux)  

### 面试题
[知乎1](https://zhuanlan.zhihu.com/p/161100568)  
[Linux内核知识点---线程、锁、中断下半部、定时器](https://blog.csdn.net/baidu_19348579/article/details/126222242)  
