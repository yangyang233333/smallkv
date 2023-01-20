//
// Created by qianyy on 2023/1/17.
//
#include <memory>
#include <gtest/gtest.h>
#include "../src/table/footer_builder.h"

namespace smallkv {
    TEST(FooterBuilder, basic) {
        /*
         * Footer模块_data数据schema如下所示：
         *         +---------------------------+----------------------------+
         *         | MetaBlock_OffsetInfo (8B) | IndexBlock_OffsetInfo (8B) |
         *         +---------------------------+----------------------------+
         * */
        auto footerBuilder = std::make_unique<FooterBuilder>();
        OffsetInfo MetaBlock_OffsetInfo{1, 2}, IndexBlock_OffsetInfo{3, 4};
        footerBuilder->add(MetaBlock_OffsetInfo, IndexBlock_OffsetInfo);
        auto data = footerBuilder->data().data();
        auto first = reinterpret_cast<const OffsetInfo *>(data);
        auto second = reinterpret_cast<const OffsetInfo *>(data + 8);
        EXPECT_EQ(first->size, 1);
        EXPECT_EQ(first->offset, 2);
        EXPECT_EQ(second->size, 3);
        EXPECT_EQ(second->offset, 4);
        EXPECT_EQ(footerBuilder->data().size(), 16);
        footerBuilder->clear();
        EXPECT_EQ(footerBuilder->data().size(), 0);
    }
}