//
// Created by qianyy on 2023/1/25.
//
#include "skiplist.h"
#include "memtable.h"

#include <utility>
#include "utils/codec.h"
#include "log/log.h"
#include "table/sstable_builder.h"
#include "memtable_iterator.h"

namespace smallkv {
    MemTable::MemTable(std::shared_ptr<FreeListAllocate> alloc) : alloc(std::move(alloc)) {
        // todo: 优化点：此处应该使用string_view
        ordered_table_ = std::make_shared<SkipList<std::string, std::string>>(alloc);
        logger = log::get_logger();
    }

    void MemTable::Insert(OpType op_type,
                          const std::string_view &key,
                          const std::string_view &value) {
        if (op_type == OpType::kAdd) {
            // 新数据插入
            ordered_table_->Insert(key.data(), value.data());
        } else if (op_type == OpType::kUpdate) {
            // todo: 对于原地更新，最好在skiplist中直接提供接口，而不是采用Del-Add的低效方法。
            // 旧数据原地更新
            ordered_table_->Delete(key.data());
            ordered_table_->Insert(key.data(), value.data());
        } else if (op_type == OpType::kDeletion) {
            // todo 同理，建议直接在skiplist中提供接口
            if (ordered_table_->Contains(key.data())) {
                ordered_table_->Delete(key.data());
            }
            ordered_table_->Insert(key.data(), ""); // value="" 表示删除
        } else {
            logger->error("Unexpected op_type. op_type={}", op_type);
            return;
        }
    }

    bool MemTable::Contains(const std::string_view &key) {
        return ordered_table_->Contains(key.data());
    }

    std::optional<std::string> MemTable::Get(const std::string_view &key) {
        return ordered_table_->Get(key.data());
    }

    void MemTable::MemTableToL1SST(const std::string &sst_filepath,
                                   std::shared_ptr<SSTableBuilder> sstable_builder) {

//        sstable_builder->add();


    }

    MemTableIterator *MemTable::NewIter() {
        return new MemTableIterator(this->ordered_table_.get());
    }

    int64_t MemTable::GetMemUsage() {
        return ordered_table_->GetMemUsage();
    }
}