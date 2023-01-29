//
// Created by qianyy on 2023/1/29.
//
#include <memory>
#include <gtest/gtest.h>
#include <string>
#include "db/options.h"
#include "db/db.h"
#include "db/db_impl.h"

namespace smallkv::unittest {
    TEST(DB, Put_Get) {
        auto logger = log::get_logger();
        auto test_options = MakeOptionsForDebugging();
        auto db_holder = std::make_unique<DB>(test_options);
        WriteOptions wOp;
        ReadOptions rOp;
        // 生成测试数据
        const int N = 1000;
        std::vector<std::string> data_key, data_val;
        for (int i = 0; i < N; ++i) {
            data_key.push_back("key_" + std::to_string(i));
            data_val.push_back("val_" + std::to_string(i));
        }
        std::sort(data_key.begin(), data_key.end());
        std::sort(data_val.begin(), data_val.end());

        // 插入数据
        for (int i = 0; i < N; ++i) {
            db_holder->Put(wOp, data_key[i], data_val[i]);
        }

        // 检查数据
        std::string *value = new std::string();
        for (int i = 0; i < N; ++i) {
            EXPECT_EQ(db_holder->Get(rOp, data_key[i], value), Status::Success);
            EXPECT_EQ(*value, data_val[i]);
            value->clear();
        }

        db_holder->Close();
    }
}