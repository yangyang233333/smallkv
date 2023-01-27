//
// Created by qianyy on 2023/1/16.
//
#include <memory>
#include <gtest/gtest.h>
#include <string>
#include "../src/table/index_block_builder.h"
#include "../src/log/log.h"
#include "../src/utils/codec.h"
#include "../src/db/offset_info.h"

namespace smallkv::unittest {
    TEST(index_block_builder, add) {
        auto logger = log::get_logger();
        auto builder = std::make_unique<IndexBlockBuilder>();
        /*
         * _data的数据排布如下：
         *         +------------------------+---------------+-----------------+
         *         | _shortest_key_size(4B) | _shortest_key | _offsetInfo(8B) |
         *         +------------------------+---------------+-----------------+
         *
         * */
        OffsetInfo key_0_offset{1, 2};
        builder->add("key_0", key_0_offset);
        auto data = builder->data();
        builder->clear();

        auto _shortest_key_size = utils::DecodeFixed32(data.c_str());
        std::string _shortest_key = data.substr(4, _shortest_key_size);
        auto int64 = utils::DecodeFixed64(data.c_str() + _shortest_key_size + 4);
        OffsetInfo _offsetInfo = *reinterpret_cast<const OffsetInfo *>(&int64);

        EXPECT_EQ(_shortest_key_size, 5);
        EXPECT_EQ(_shortest_key, "key_0");
        EXPECT_EQ(_offsetInfo.size, 1);
        EXPECT_EQ(_offsetInfo.offset, 2);
    }
}