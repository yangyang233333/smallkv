//
// Created by qianyy on 2022/12/14.
//
#include <gtest/gtest.h>
#include "../src/utils/murmur_hash2.h"

namespace smallkv {
    TEST(MurmurHash, basic) {
        EXPECT_EQ(utils::murmur_hash2("123", 3), 1023762986);
    }
}