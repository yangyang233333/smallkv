//
// Created by qianyy on 2023/1/26.
//
#include <iostream>
#include <ctime>
#include <memory>
#include <string>
#include <gtest/gtest.h>
#include "memtable/memtable.h"
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
}
