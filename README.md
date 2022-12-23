# smallkv

这个项目主要源于一次阿里云的newsql数据库大赛参赛经历，
由于比赛中写的架构较为简略，所以在这里重构一下这个kv引擎。

## 进度

- [ ] 跳表
- [x] 布隆过滤器
- [x] 内存池
- [ ] 缓存
- [ ] 磁盘结构（SST+MANIFEST）
- [ ] Compact

## 设计
1. 内存池设计

![mem_pool](./img/mem_pool_design.png)
## build

```shell
./build.sh
./run_main.sh
```

## 第三方依赖：

1. [spdlog](https://github.com/gabime/spdlog)
2. [gtest](https://github.com/google/googletest)
3. [rapidjson](https://github.com/Tencent/rapidjson)

## 参考项目：

1. [阿里云NewSQL数据库大赛](https://tianchi.aliyun.com/competition/entrance/531980/introduction)
2. [corekv](https://github.com/hardcore-os/coreKV-CPP)
3. [leveldb](https://github.com/google/leveldb)
