# smallkv

这个项目主要源于一次阿里云的newsql数据库大赛参赛经历，
由于比赛中写的架构较为简略，所以在这里重构一下这个kv引擎。

## 进度

- [ ] 跳表
- [x] 布隆过滤器
- [x] 内存池
- [x] 缓存
- [ ] 磁盘结构（SST+MANIFEST）
- [ ] Compact

## 设计
1. 内存池设计

![mem_pool](./img/mem_pool_design.png)

2. 缓存设计
![cache](./img/cache_design.png)
Cache中持有N（默认为5）个指向CachePolicy的指针，相当于5个分片，可以减少哈希冲突以及减少锁的范围；LRUCache和LFUCache都是CachePolicy的子类。

## build

```shell
./build.sh
./main_run.sh      ## 主程序
./unittest_run.sh  ## 单元测试
```

## 第三方依赖：

1. [spdlog](https://github.com/gabime/spdlog)
2. [gtest](https://github.com/google/googletest)
3. [nlohmann/json](https://github.com/nlohmann/json)

## 参考项目：

1. [阿里云NewSQL数据库大赛](https://tianchi.aliyun.com/competition/entrance/531980/introduction)
2. [corekv](https://github.com/hardcore-os/coreKV-CPP)
3. [leveldb](https://github.com/google/leveldb)
