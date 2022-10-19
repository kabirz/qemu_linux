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
