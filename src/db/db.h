//
// Created by qianyy on 2023/1/27.
//
#include <memory>
#include <string_view>
#include "status.h"
#include "options.h"

#ifndef SMALLKV_DB_H
#define SMALLKV_DB_H
namespace smallkv {
    class DBImpl;

    class DB {
    public:
        explicit DB(const Options& options);

        ~DB() = default;

        // DB 应该是单例，禁止拷贝、赋值
        DB(const DB &) = delete;

        DB &operator=(const DB &) = delete;

        DBStatus Put(const WriteOptions &options,
                     const std::string_view &key,
                     const std::string_view &value);

        DBStatus Delete(const WriteOptions &options,
                        const std::string_view &key);

        // 将Key对应的值写到value地址上
        DBStatus Get(const ReadOptions &options,
                     const std::string_view &key,
                     std::string *value);

        // 批写
        DBStatus BatchPut(const WriteOptions &options);

        DBStatus BatchDelete(const ReadOptions &options);

        // 关闭数据库：调用此函数可以保证所有已写入数据会被持久化到磁盘，
        DBStatus Close();

    private:
        std::unique_ptr<DBImpl> db_impl;
    };
}
#endif //SMALLKV_DB_H
