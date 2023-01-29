//
// Created by qianyy on 2023/1/26.
//
#include <iostream>
#include <ctime>
#include <memory>
#include <string>
#include <gtest/gtest.h>
#include "memtable/memtable.h"
#include "memtable/memtable_iterator.h"
#include "memory/allocate.h"

namespace smallkv::unittest {
    TEST(MemTable, Insert__Add) {
        auto alloc = std::make_shared<FreeListAllocate>();
        auto mem_table = std::make_shared<MemTable>(alloc);

        constexpr int N = 1234;
        for (int i = 0; i < N; ++i) {
            auto key = "key_" + std::to_string(i);
            auto val = "val_" + std::to_string(i);
            mem_table->Add(key, val);
        }
        for (int i = 0; i < N; ++i) {
            auto key = "key_" + std::to_string(i);
            auto val = "val_" + std::to_string(i);
            EXPECT_EQ(mem_table->Get(key), val);
        }
    }

    // 插入重复值
    TEST(MemTable, Insert__add_update) {
        auto alloc = std::make_shared<FreeListAllocate>();
        auto mem_table = std::make_shared<MemTable>(alloc);

        srand(time(nullptr));
        constexpr int N = 1234;

        for (int i = 0; i < N; ++i) {
            auto key = "key_" + std::to_string(i);
            auto val = "val_" + std::to_string(i);
            mem_table->Add(key, val);
            // 随机插入重复值
            if (rand() & 1) {
                auto duplicate_val = "duplicate_" + val;
                mem_table->Update(key, duplicate_val);
                EXPECT_EQ(mem_table->Get(key), duplicate_val);
            } else {
                EXPECT_EQ(mem_table->Get(key), val);
            }
        }
    }

    TEST(MemTable, Insert__Deletion) {
        auto alloc = std::make_shared<FreeListAllocate>();
        auto mem_table = std::make_shared<MemTable>(alloc);

        srand(time(nullptr));
        constexpr int N = 1234;

        for (int i = 0; i < N; ++i) {
            auto key = "key_" + std::to_string(i);
            auto val = "val_" + std::to_string(i);
            mem_table->Add(key, val);
            // 随机插入重复值
            if (rand() & 1) {
                auto duplicate_val = "duplicate_" + val;
                mem_table->Update(key, duplicate_val);
                EXPECT_EQ(mem_table->Get(key), duplicate_val);
            } else {
                EXPECT_EQ(mem_table->Get(key), val);
            }

            // 随机删除
            if (rand() & 1) {
                mem_table->Delete(key);
                EXPECT_EQ(mem_table->Get(key), "");
            }
        }
    }

    TEST(MemTable, MemTableIterator) {
        auto alloc = std::make_shared<FreeListAllocate>();
        auto mem_table = std::make_shared<MemTable>(alloc);

        const int N = 1000;
        // 构建插入数据
        std::vector<std::string> data_key, data_value;
        for (int i = 0; i < N; ++i) {
            data_key.emplace_back("key_" + std::to_string(i));
            data_value.emplace_back("value_" + std::to_string(i));
        }

        std::sort(data_key.begin(), data_key.end());
        std::sort(data_value.begin(), data_value.end());

        // 插入
        for (int i = 0; i < N; ++i) {
            mem_table->Add(data_key[i], data_value[i]);
        }

        auto iter = mem_table->NewIter();
        iter->MoveToFirst();
        // 测试迭代器
        for (int i = 0; i < N; ++i) {
            EXPECT_EQ(iter->key(), data_key[i]);
            EXPECT_EQ(iter->value(), data_value[i]);
            iter->Next();
        }
    }
}
