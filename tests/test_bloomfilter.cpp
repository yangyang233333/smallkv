//
// Created by qianyy on 2022/12/13.
//
#include <memory>
#include <gtest/gtest.h>
#include "../src/filter/bloom_filter.h"
#include "../src/filter/filter_policy.h"

namespace smallkv::unittest {
    TEST(BloomFilter, basic) {
        std::vector<std::string> data;
        for (int i = 0; i < 10 * 10000; ++i) {
            data.push_back("key_" + std::to_string(i));
        }
        // 预期10w条数据，假阳性概率为1%
        std::unique_ptr<FilterPolicy> filterPolicy = std::make_unique<BloomFilter>(10 * 10000, 0.01);
        filterPolicy->create_filter(data);
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

    TEST(BloomFilter, basic2) {
        std::vector<std::string> data;
        for (int i = 0; i < 100 * 10000; ++i) {
            data.push_back("key_" + std::to_string(i));
        }
        std::unique_ptr<FilterPolicy> filterPolicy = std::make_unique<BloomFilter>(100 * 10000, 0.001);
        filterPolicy->create_filter(data);
        int cnt = 0;
        for (int i = 0; i < 100 * 10000; ++i) {
            if (filterPolicy->exists("key_" + std::to_string(i))) {
                ++cnt;
            }
        }
        EXPECT_EQ(100 * 10000, cnt);
        cnt = 0;
        for (int i = 100 * 10000; i < 200 * 10000; ++i) {
            if (filterPolicy->exists("key_" + std::to_string(i))) {
                ++cnt;
            }
        }
        EXPECT_LT(cnt, 3000);
    }
}