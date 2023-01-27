//
// Created by qianyy on 2023/1/25.
//
#include <memory>
#include <string>
#include "memtable_config.h"
#include "op_type.h"
#include "log/log.h"

#ifndef SMALLKV_MEMTABLE_H
#define SMALLKV_MEMTABLE_H

namespace smallkv {
    template<typename Key, typename Value>
    class SkipList;

    class FreeListAllocate;

    /*
     * Insert逻辑：
     * 1. Add key, OpType=kAdd
     *     1.1 active memtable中不存在key，正常添加即可；
     *     1.2 active memtable中存在key，进行原地更新。
     *
     * 2. Delete key, OpType=kDeletion
     *     2.1 active memtable中不存在key，相当于插入<key, nil>
     *     2.2 active memtable中存在key，原地更新为<key, nil>
     *
     *
     * */
    class MemTable final {
    public:
        explicit MemTable(std::shared_ptr<FreeListAllocate> alloc);

        ~MemTable() = default;

        MemTable() = delete;

        // 禁用拷贝、赋值。
        MemTable(const MemTable &) = delete;

        MemTable &operator=(const MemTable &) = delete;

        inline void Add(const std::string_view &key,
                        const std::string_view &value) {
            this->Insert(OpType::kAdd, key, value);
        }

        inline void Update(const std::string_view &key,
                           const std::string_view &value) {
            this->Insert(OpType::kUpdate, key, value);
        }

        inline void Delete(const std::string_view &key) {
            this->Insert(OpType::kDeletion, key, "");
        }

        inline int64_t GetMemUsage() { return ordered_table_->GetMemUsage(); }

        bool Contains(const std::string_view &key);

        // 如果不存在则返回nullopt
        std::optional<std::string> Get(const std::string_view &key);

    private:
        // Add、Update、Delete都属于Insert
        // 如果是Delete，则value=""
        // OpType = {kDeletion, kAdd, kUpdate}
        void Insert(OpType op_type, const std::string_view &key,
                    const std::string_view &value);

    private:
        std::shared_ptr<SkipList<std::string, std::string>> ordered_table_;
        std::shared_ptr<FreeListAllocate> alloc;
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif //SMALLKV_MEMTABLE_H
