//
// Created by qianyy on 2023/1/16.
//
#include <memory>
#include <gtest/gtest.h>
#include <string>
#include "../src/table/data_block_builder.h"
#include "../src/log/log.h"
#include "../src/utils/codec.h"
#include "../src/db/offset_info.h"

namespace smallkv::unittest {
    TEST(data_block_builder, add) {
        auto logger = log::get_logger();
        auto builder = std::make_unique<DataBlockBuilder>();

        /*
         * 每条记录的前面有三个int32_t， 然后才是key、value
         * Record的schema如下：
         *    +--------------------+----------------------+---------------+----------------------+---------------+
         *    | shared_key_len(4B) | unshared_key_len(4B) | value_len(4B) | unshared_key_content | value_content |
         *    +--------------------+----------------------+---------------+----------------------+---------------+
         *
         * */
        builder->add("key_0", "val_0");
        builder->add("key_1", "val_1");
        builder->add("key_2", "val_2");
        EXPECT_EQ(builder->data().substr(12, 10), "key_0val_0");
        EXPECT_EQ(builder->data().substr(34, 6), "1val_1");
        EXPECT_EQ(builder->data().substr(52, 6), "2val_2");
        EXPECT_EQ(builder->data().size(), 12 + 10 + 12 + 6 + 12 + 6);
    }

    TEST(data_block_builder, add_restart_points) {
        auto logger = log::get_logger();
        auto builder = std::make_unique<DataBlockBuilder>();
        builder->add("key_0", "val_0");
        builder->add("key_1", "val_1");
        builder->add("key_2", "val_2");
        EXPECT_EQ(builder->data().substr(12, 10), "key_0val_0");
        EXPECT_EQ(builder->data().substr(34, 6), "1val_1");
        EXPECT_EQ(builder->data().substr(52, 6), "2val_2");
        EXPECT_EQ(builder->data().size(), 58); // 插入了三条数据，此时全部size应该为58
        /*
         *     Restart_Point的schema如下：
         *            +----------------+----------------+
         *            | record_num(4B) | OffsetInfo(8B) |
         *            +----------------+----------------+
         *
         * */
        // 添加重启点
        builder->finish_data_block();
        EXPECT_EQ(builder->data().size(), 74); // 加上重启点和Restart Point Num后，全体size应该为58+12+4=74B
        EXPECT_EQ(utils::DecodeFixed32(builder->data().substr(58, 4).data()), 3); // record_num == 3
        OffsetInfo offsetInfo = *reinterpret_cast<const OffsetInfo *>(
                builder->data().substr(62, 8).data()); // OffsetInfo
        EXPECT_EQ(offsetInfo.size, 58); // 第一个Record Group的大小
        EXPECT_EQ(offsetInfo.offset, 0); // 第一个Record Group的偏移量

        EXPECT_EQ(utils::DecodeFixed32(builder->data().substr(70, 4).data()), 1); // restart_point_num == 1
    }

    TEST(data_block_builder, all) {
        //todo: 需要写一个iterator模块，来遍历data_block，否则debug太复杂

        // todo：测试大数据量情况下的add和add_restart_points


    }
}