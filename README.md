# smallkv
![](https://github.com/yangyang233333/smallkv/actions/workflows/cmake-unittest.yml/badge.svg)
![GitHub top language](https://img.shields.io/github/languages/top/yangyang233333/smallkv)
[![GitHub stars](https://img.shields.io/github/stars/yangyang233333/smallkv)](https://github.com/yangyang233333/smallkv)
[![LICENSE](https://img.shields.io/github/license/yangyang233333/smallkv.svg?style=flat-square)](https://github.com/yangyang233333/smallkv/blob/master/LICENSE)

这个项目主要源于一次阿里云的newsql数据库大赛参赛经历，
由于比赛中写的架构较为简略，所以在这里重构一下这个kv引擎。
smallkv 是一个列存的、基于LSM架构的存储引擎。

## 进度

- [ ] memtable: 跳表
- [x] 布隆过滤器
- [x] 内存池
- [x] 缓存模块
- [x] FileWriter/FileReader(todo: 支持mmap)
- [ ] 磁盘模块（SST+MANIFEST）
- [ ] WAL模块
- [ ] Compaction模块

## 设计
1. 内存池设计

![mem_pool](./img/mem_pool_design.png)

2. 缓存设计
![cache](./img/cache_design.png)
Cache中持有N（默认为5）个指向CachePolicy的指针，相当于5个分片，可以减少哈希冲突以及减少锁的范围；LRUCache和LFUCache都是CachePolicy的子类。

## build

必须使用g++编译器
### build from source code:
```shell
# 安装依赖
apt update && apt upgrade -y && apt install cmake make git g++ gcc -y && cd ~ \
    && git clone https://github.com/gabime/spdlog.git && cd spdlog && mkdir build && cd build && cmake .. && make -j && sudo make install && cd ~ \
    && git clone https://github.com/google/googletest && cd googletest && mkdir build && cd build && cmake .. && make -j && sudo make install && cd ~ \
    && git clone https://github.com/nlohmann/json && cd json && mkdir build && cd build && cmake .. && make -j && sudo make install && cd ~ \
    && rm -rf spdlog googletest json
./build.sh         ##  编译
./main_run.sh      ## 主程序
./unittest_run.sh  ## 单元测试
```
### build from docker (Highly recommended)
```shell
cd docker
docker build -t smallkv-testenv . # 需要几分钟
docker run -it -v /{smallkv代码所在的目录}:/test smallkv-testenv /bin/bash
./build.sh         ##  编译
./main_run.sh      ## 主程序
./unittest_run.sh  ## 单元测试
```

## 第三方依赖：

1. [spdlog](https://github.com/gabime/spdlog)
2. [gtest](https://github.com/google/googletest)
3. [nlohmann/json](https://github.com/nlohmann/json)

## 参考：

1. [阿里云NewSQL数据库大赛](https://tianchi.aliyun.com/competition/entrance/531980/introduction)
2. [corekv](https://github.com/hardcore-os/coreKV-CPP)
3. [leveldb](https://github.com/google/leveldb)
4. [LSM树原理](https://zhuanlan.zhihu.com/p/181498475)
5. [LSM Tree是什么?](https://www.zhihu.com/question/446544471/answer/2348883977)

---

感谢 [JetBrains](https://jb.gg/OpenSourceSupport) 捐献的免费许可证帮助我们开发smallkv。  
Thanks to [JetBrains](https://jb.gg/OpenSourceSupport) for donating product licenses to help develop **smallkv** <a href="https://jb.gg/OpenSourceSupport"><img src="img/jb_beam.svg" width="94" align="center" /></a>
