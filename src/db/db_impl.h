//
// Created by qianyy on 2023/1/28.
//
#include <memory>
#include <string_view>
#include <shared_mutex>
#include "status.h"
#include "options.h"
#include "log/log.h"

#ifndef SMALLKV_DB_IMPL_H
#define SMALLKV_DB_IMPL_H

namespace smallkv {
    template<typename K, typename V>
    class Cache;

    class MemTable;

    class WALWriter;

    class FreeListAllocate;

    /*
     * 支持并发，线程安全
     *
     * */
    class DBImpl {
    public:
        explicit DBImpl(Options options);

        ~DBImpl() = default;

        // 同时具备Set和Update语义
        DBStatus Put(const WriteOptions &options,
                     const std::string_view &key,
                     const std::string_view &value);

        DBStatus Delete(const WriteOptions &options,
                        const std::string_view &key);

        // 将Key对应的值写到value地址上
        DBStatus Get(const ReadOptions &options,
                     const std::string_view &key,
                     std::string *ret_value_ptr);

        // 关闭数据库：调用此函数可以保证所有已写入数据会被持久化到磁盘，
        DBStatus Close();

        // 批写
        DBStatus BatchPut(const WriteOptions &options);

        DBStatus BatchDelete(const ReadOptions &options);

    private:
        // 将 KV 编码到 buf 中, 必须确保buf长度为8 + key.size() + value.size()
        static void EncodeKV(const std::string_view &key,
                             const std::string_view &value,
                             char *buf);

        // 将memtable转为sst
        void MemTableToSST();

    private:
        std::shared_ptr<MemTable> mem_table;       // active memtable
        std::shared_ptr<spdlog::logger> logger;    // 日志
        std::shared_ptr<FreeListAllocate> alloc;   // 内存分配器
        std::shared_ptr<WALWriter> wal_writer;     // 写wal

        std::shared_ptr<Cache<std::string, std::string>> cache;     // 缓存

        Options options_;                          // 配置信息

        std::shared_mutex rwlock_;                 // 读写锁

        bool closed = false;                       // 表示数据库没有关闭
    };
}

#endif //SMALLKV_DB_IMPL_H
