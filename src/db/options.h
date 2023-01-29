//
// Created by qianyy on 2023/1/27.
//
#include <string>

#ifndef SMALLKV_OPTIONS_H
#define SMALLKV_OPTIONS_H
namespace smallkv {
    // DB的配置信息，如是否开启同步、缓存池等
    struct Options {
        //todo: 之前的配置信息已经写到了xxx_config中，后续应该集中到这里

        // 数据库的存储目录，需要自定义
//        std::string DB_DIR = "./output/db_storage";
        std::string STORAGE_DIR = "./output/db_storage";

        // WAL的存储路径，需要自定义
        std::string WAL_DIR = "./output/wal_log.txt";

        // MEM_TABLE的最大大小，超过了就应该落盘
        size_t MEM_TABLE_MAX_SIZE = 4 * 1024 * 1024; // 4MB

        // 缓存的键值对数量
        uint32_t CACHE_SIZE = 4096;

        // 表示当前L1SST的序号。 L1SST的命名类似level_1_sst_0.sst, level_1_sst_1.sst, ....
        // 开始的时候需要扫描 STORAGE_DIR 目录，找到下一个sst的LISST_NUM
        uint32_t LISST_NUM = 0;
    };

    inline Options MakeOptionsForDebugging() {
        return Options{};
    }

    inline Options MakeOptionsForProduction() {
        return Options{};
    }

    // 读时候的配置信息
    struct ReadOptions {
        // 扩展性备用接口。
    };

    //写时候的配置信息
    struct WriteOptions {
        /*
         * 注：C库缓冲 --fflush--> 内核缓冲 --fsync--> 磁盘
         * 解释：
         * 1. fsync系统调用可以强制每次写入都被更新到磁盘中，在open()中添加O_SYNC也由此效果；
         * 2. fflush是一个在C语言标准输入输出库中的函数，功能是冲洗流中的信息，该函数通常用于
         *    处理磁盘文件。fflush()会强迫将缓冲区内的数据写回参数stream 指定的文件中。
         * 一般地，fsync也不能保证100%安全，因为现在的磁盘也有缓存（比如固态硬盘可能有外置DRAM缓存），
         * 如果断电数据也可能会丢失。但是企业级硬盘一般有备用电源，并且很多固态的缓存是用的SLC颗粒(断电不丢失)，
         * 所以基本可以认为fsync可以保证数据安全。
         *
         * */
        // 此处的flush和fflush语义相同，实际上flush不需要设置为true，因为WAL已经保证了数据安全(fsync)。
        // todo: Flush这个开关暂时无效，后续有空实现
        bool Flush = false;
    };
}
#endif //SMALLKV_OPTIONS_H
