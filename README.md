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
[linux 内核笔记](https://blog.csdn.net/u012830148/category_7622076.html)  
[Linux操作系统任务调度的实现](https://zhuanlan.zhihu.com/p/393968249)  
[进程调度之实时调度策略（SCHED_FIFO、SCHED_RR、MAX_RT_PRIO实时优先级）](https://blog.csdn.net/qq_41453285/article/details/103757542)  
[linux进程调度方法(SCHED_OTHER,SCHED_FIFO,SCHED_RR)](https://blog.csdn.net/cjianeng/article/details/115510193)  
[知乎-Linux内核库](https://www.zhihu.com/people/cheng-xu-yuan-mian-shi-zhi-nan/posts?page=1)  
[github-linux device driver](https://github.com/PacktPublishing/Linux-Device-Drivers-Development)  
[CSDN 董哥的黑板报-Linux内核解剖](https://blog.csdn.net/qq_41453285/category_9612179.html)  
[v4l2 -vimc](https://www.kernel.org/doc/html/v5.4/media/v4l-drivers/vimc.html)  
[大神-算法相关的学习资源](https://programmercarl.com/)  


[mtk linux](https://gitlab.com/mediatek/aiot/bsp/linux)  

### 面试题
[知乎1](https://zhuanlan.zhihu.com/p/161100568)  
[Linux内核知识点---线程、锁、中断下半部、定时器](https://blog.csdn.net/baidu_19348579/article/details/126222242)  

### B 站视频
[庖丁解牛Linux内核](https://www.bilibili.com/video/BV1yD4y1m7Q9)  
[嵌入式底层驱动开发](https://www.bilibili.com/video/BV1Tp4y1S7Jh)  

### CAN
[物联网CSDN](https://edu.csdn.net/course/2-377-0-0-1.html)  
[【汽车总线技术】CAN总线技术基础讲解合集](https://www.bilibili.com/video/BV1gD4y1U7bJ)  
[CAN通信速成！从会到不会~](https://www.bilibili.com/video/BV1Dq4y1J7WA)  
[Linux下CAN总线是如何使用的](https://zhuanlan.zhihu.com/p/537791552)  



