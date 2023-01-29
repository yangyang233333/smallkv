//
// Created by qianyy on 2023/1/28.
//

#include <utility>

#include "db_impl.h"
#include "cache/cache.h"
#include "utils/codec.h"
#include "memory/allocate.h"
#include "memtable/memtable.h"
#include "wal/wal_writer.h"
#include "file/file_writer.h"
#include "table/sstable_builder.h"

namespace smallkv {
    DBImpl::DBImpl(Options options) : options_(std::move(options)) {
        alloc = std::make_shared<FreeListAllocate>();
        mem_table = std::make_shared<MemTable>(alloc);
        logger = log::get_logger();

        wal_writer = std::make_shared<WALWriter>(std::make_shared<FileWriter>(options_.WAL_DIR));

        cache = std::make_shared<Cache<std::string, std::string>>(options_.CACHE_SIZE);
        cache->register_clean_handle([](const std::string &key, std::string *val) {
            delete val;
        });
    }

    DBStatus DBImpl::Put(const WriteOptions &options,
                         const std::string_view &key,
                         const std::string_view &value) {
        assert(closed == false);
        /*
         * 写逻辑：
         * 1. 写WAL(fsync同步);
         * 2. 写memtable;
         * 3. 写缓存(提高读性能);
         * 4. 如果memtable超限，应该落盘，并且开启一个新的memtable;
         *
         * */
        std::unique_lock<std::shared_mutex> wlock(rwlock_);

        // 1. 写WAL
        char buf[8 + key.size() + value.size()];
        EncodeKV(key, value, buf); // 将K-V编码到buf中
        wal_writer->AddLog(buf);

        // 2. 写memtable
        if (mem_table->Contains(key)) { // Update
            mem_table->Update(key, value);
        } else { // New Insert
            mem_table->Add(key, value);
        }

        // 3. 写缓存
        // todo: 写入时候不一定需要写入缓存. 如果一次性写入大量数据，实际上不需要每次
        //  都更新缓存，可以设置一种动态的、热点感知的缓存机制。后续有空优化。
        // todo: 此处采用new std::string()性能很差，后续需要修改底层的cache接口。
        cache->insert(key.data(), new std::string(value.data()));

        // 4. 判断MemTable是否超限, 如果超限应该转为L1SST后持久化
        if (mem_table->GetMemUsage() >= options_.MEM_TABLE_MAX_SIZE) {
            MemTableToSST(); // 将memtable转为sst

            // 开启写的memtable
            mem_table = std::make_shared<MemTable>(alloc);
            logger->info("[DBImpl::Put] A new mem_table is created.");
        }
        return Status::Success;
    }

    DBStatus DBImpl::Delete(const WriteOptions &options,
                            const std::string_view &key) {
        assert(closed == false);
        /*
         * 删除逻辑：
         * 1. 写WAL;
         * 2. 写memtable;
         * 3. 删除缓存;
         * 4. 如果memtable超限，应该落盘，并且开启一个新的memtable;
         * */
        std::unique_lock<std::shared_mutex> wlock(rwlock_);

        // 1. 写WAL
        char buf[8 + key.size()]; // 用vel_len=0表示val为空
        EncodeKV(key, "", buf);
        wal_writer->AddLog(buf);

        // 2. 写memtable
        if (mem_table->Contains(key)) { // 原地标记val=""表示删除
            mem_table->Delete(key);
        } else {
            mem_table->Add(key, ""); // 墓碑机制
        }

        // 3. 删除缓存
        cache->erase(key.data());

        // 4. 检查memtable是否超限
        if (mem_table->GetMemUsage() >= options_.MEM_TABLE_MAX_SIZE) {
            MemTableToSST(); // 将memtable转为sst

            // 开启写的memtable
            mem_table = std::make_shared<MemTable>(alloc);
            logger->info("[DBImpl::Delete] A new mem_table is created.");
        }
        return Status::Success;
    }

    DBStatus DBImpl::Get(const ReadOptions &options,
                         const std::string_view &key,
                         std::string *value) {
        assert(closed == false);
        /*
         * 读逻辑：
         * 1. 读缓存，有则直接返回，否则进入2;
         * 2. 依次从memtable、sst文件向下查找;
         * 3. 找到的数据写入缓存;
         * 4. 返回结果;
         *
         * */
        std::shared_lock<std::shared_mutex> rlock(rwlock_);

        // 1. 读缓存
        if (cache->contains(key.data())) {
            *value = *(cache->get(key.data())->val);
            return Status::Success;
        }

        // 2. 读memtable
        if (mem_table->Contains(key)) {
            auto val = mem_table->Get(key);
            *value = mem_table->Get(key.data()).value();
            return Status::Success;
        }

        // 3. 依次读sst文件
        // todo: 后续实现

        // 4. 找到的数据写入缓存
        // todo


        return Status::ExecFailed;
    }

    DBStatus DBImpl::BatchPut(const WriteOptions &options) {
        std::unique_lock<std::shared_mutex> wlock(rwlock_);
        assert(closed == false);
        // todo: 稍后实现
        return Status::NotImpl;
    }

    DBStatus DBImpl::BatchDelete(const ReadOptions &options) {
        std::unique_lock<std::shared_mutex> wlock(rwlock_);
        assert(closed == false);
        // todo: 稍后实现
        return Status::NotImpl;
    }

    void DBImpl::EncodeKV(const std::string_view &key,
                          const std::string_view &value,
                          char *buf) {
        /*
         * 暂时采用的编码方法如下：
         *     +-------------+-----+-------------+-----+
         *     | key_len(4B) | key | val_len(4B) | val |
         *     +-------------+-----+-------------+-----+
         * todo: 存在优化空间，例如使用variant等，后续有空再说
         *
         * */
        assert(value.size() < UINT32_MAX);
        utils::EncodeFixed32(buf, key.size());
        memcpy(buf + 4, key.data(), key.size());
        utils::EncodeFixed32(buf + 4 + key.size(), value.size());
        memcpy(buf + 4 + key.size() + 4, value.data(), value.size());
    }

    void DBImpl::MemTableToSST() {
        // todo: 此处采用同步方法(为了debug方便)，后续需要修改为异步

        // 格式为/.../level_n_sst_i.sst
        auto sst_filepath = options_.STORAGE_DIR + "/" + utils::BuildSSTPath(0, options_.LISST_NUM);
        logger->info("DBImpl::MemTableToSST() is called. sst_filepath={}", sst_filepath);

        auto file_writer = std::make_shared<FileWriter>(sst_filepath);
        auto sstable_builder = std::make_shared<SSTableBuilder>(mem_table->GetSize(), file_writer);
        mem_table->ConvertToL1SST(sst_filepath, sstable_builder);

        ++options_.LISST_NUM; // 下一个sst文件序号+1
    }

    DBStatus DBImpl::Close() {
        if (!closed && mem_table->GetSize() > 0) {
            // memtable中有数据，就应该落盘
            MemTableToSST();

            closed = true;
        }
        logger->info("DB is closed.");
        return Status::Success;
    }
}
