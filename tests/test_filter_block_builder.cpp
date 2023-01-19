//
// Created by qianyy on 2023/1/17.
//
#include <memory>
#include <gtest/gtest.h>
#include "../src/table/filter_block_builder.h"

namespace smallkv {
    TEST(filter_block_builder, basic) {
        std::vector<std::string> data;
        for (int i = 0; i < 10 * 10000; ++i) {
            data.emplace_back("key_" + std::to_string(i));
        }
        // 预期10w条数据，假阳性概率为1%
        std::unique_ptr<FilterBlockBuilder> filterPolicy = std::make_unique<FilterBlockBuilder>(10 * 10000, 0.01);
        for (const auto &d: data) {
            filterPolicy->add(d);
        }
        filterPolicy->finish_filter_block();
        int cnt = 0;
        for (int i = 0; i < 10 * 10000; ++i) {
            if (filterPolicy->exists("key_" + std::to_string(i))) {
                ++cnt;
            }
        }
        EXPECT_EQ(10 * 10000, cnt);
        cnt = 0;
        for (int i = 10 * 10000; i < 20 * 10000; ++i) {
            if (filterPolicy->exists("key_" + std::to_string(i))) {
                ++cnt;
            }
        }
        EXPECT_LT(cnt, 3000); // cnt预期是1000（10条数据，假阳性概率为1%，预期为1000）
    }
}