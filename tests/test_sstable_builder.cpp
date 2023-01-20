//
// Created by qianyy on 2023/1/17.
//
#include <memory>
#include <gtest/gtest.h>
#include "../src/table/sstable_builder.h"
#include "../src/file/file_writer.h"
#include "../src/file/file_reader.h"
#include "../src/utils/codec.h"

namespace smallkv {
    /*
     * SST结构
     * SSTable的数据排布schema：
     *           +----------------+
     *           |   DataBlock_1  |
     *           +----------------+
     *           |       ...      |
     *           +----------------+
     *           |   DataBlock_N  |
     *           +----------------+
     *           |    MetaBlock   |
     *           +----------------+
     *           |  IndexBlock_1  |
     *           +----------------+
     *           |       ...      |
     *           +----------------+
     *           |  IndexBlock_N  |
     *           +----------------+
     *           |     Footer     |
     *           +----------------+
     *
     * DataBlock数据排布如下:
     *               +---------------+
     *               |   Record_1    |
     *               +---------------+
     *               |     ...       |
     *               +---------------+
     *               |   Record_N    |
     *               +---------------+
     *               |   Restart_1   |
     *               +---------------+
     *               |     ...       |
     *               +---------------+
     *               |   Restart_K   |
     *               +---------------+
     *               |  Restart_Num  |
     *               +---------------+
     *               | Restart_Offset|
     *               +---------------+
     *
     *     Record的schema：
     *           +--------------------+----------------------+---------------+----------------------+---------------+
     *           | shared_key_len(4B) | unshared_key_len(4B) | value_len(4B) | unshared_key_content | value_content |
     *           +--------------------+----------------------+---------------+----------------------+---------------+
     *
     *     Restart_Point的schema如下：
     *            +----------------+----------------+
     *            | record_num(4B) | OffsetInfo(8B) |
     *            +----------------+----------------+
     *
     *     IndexBlock schema
     *         +------------------------+---------------+-----------------+
     *         | _shortest_key_size(4B) | _shortest_key | _offsetInfo(8B) |
     *         +------------------------+---------------+-----------------+
     *
     *     Footer模块数据schema如下所示：
     *         +---------------------------+----------------------------+
     *         | MetaBlock_OffsetInfo (8B) | IndexBlock_OffsetInfo (8B) |
     *         +---------------------------+----------------------------+
     * */
    TEST(SSTableBuilder, basic) {
        /*
         * 本例中：
         *    SSTable：
         *    +----------------------+------------------------+-----------------+-------+-------+
         *    | shared_key_len=0(4B) | unshared_key_len=5(4B) | value_len=5(4B) | key_0 | val_0 |                     [DataBlock的Record] = 22B
         *    +----------------------+------------------------+-----------------+-------+-------+
         *    +------------------+-----------------------------------+
         *    | record_num=1(4B) | OffsetInfo={size=22,offset=0}(8B) |                                                [DataBlock的RestartPoint] = 12B
         *    +------------------+-----------------------------------+
         *    +---------------------+
         *    |  Restart_Num=1(4B)  |                                                                                 [DataBlock的Restart_Num] = 4B
         *    +---------------------+
         *    +---------------------+
         *    |        filter       |                                                                                 [MetaBlock的Filter] = 124B
         *    +---------------------+
         *    +--------------------------+---------------------+------------------------------------+
         *    | _shortest_key_size=5(4B) | _shortest_key=key_1 | _offsetInfo={size=38,offset=0}(8B) |                 [IndexBlock] = 17B
         *    +--------------------------+---------------------+------------------------------------+
         *    +------------------------------------------------+-------------------------------------------------+
         *    | MetaBlock_OffsetInfo={size=124,offset=38} (8B) | IndexBlock_OffsetInfo={size=17,offset=162} (8B) |    [Footer] = 16B
         *    +------------------------------------------------+-------------------------------------------------+
         *
         *
         * */
        auto logger = log::get_logger();
        const std::string path = "./build/test_SSTableBuilder.sst";
        auto _fileWriter = std::make_shared<FileWriter>(path);
        auto sstableBuilder = std::make_unique<SSTableBuilder>(100, _fileWriter, 0.01);
        sstableBuilder->add("key_0", "val_0");
        sstableBuilder->finish_sst();

        auto _fileReader = std::make_shared<FileReader>(path);
        char buf[195];
        _fileReader->read(buf, 195, 0);
        std::string buf_s(buf, 195);

        // 解析[DataBlock的Record] = 22B
        auto shared_key_len = static_cast<int32_t>(utils::DecodeFixed32(buf));
        auto unshared_key_len = static_cast<int32_t>(utils::DecodeFixed32(buf + 4));
        auto value_len = static_cast<int32_t>(utils::DecodeFixed32(buf + 8));
        auto key_0 = buf_s.substr(12, 5);
        auto val_0 = buf_s.substr(17, 5);
        EXPECT_EQ(shared_key_len, 0);
        EXPECT_EQ(unshared_key_len, 5);
        EXPECT_EQ(value_len, 5);
        EXPECT_EQ(key_0, "key_0");
        EXPECT_EQ(val_0, "val_0");

        // 解析 [DataBlock的RestartPoint] = 12B
        auto record_num = static_cast<int32_t>(utils::DecodeFixed32(buf + 22));
        auto Restart_Point_OffsetInfo = reinterpret_cast<OffsetInfo *>(buf + 26);
        EXPECT_EQ(record_num, 1);
        EXPECT_EQ(Restart_Point_OffsetInfo->size, 22);
        EXPECT_EQ(Restart_Point_OffsetInfo->offset, 0);

        // 解析 [DataBlock的Restart_Num] = 4B
        auto Restart_Num = static_cast<int32_t>(utils::DecodeFixed32(buf + 34));
        EXPECT_EQ(Restart_Num, 1);

        // 解析 filter
        // key_num设置为100， false_positive=0.01的情况下：
        // bits_array的长度为120，并且结尾有一个4B存储了哈希函数数量(为6)，共124B
        auto hash_func_num = static_cast<int32_t>(utils::DecodeFixed32(buf + 158));
        EXPECT_EQ(hash_func_num, 6);

        // 解析 [IndexBlock] = 17B
        auto _shortest_key_size = static_cast<int32_t>(utils::DecodeFixed32(buf + 162));
        auto _shortest_key = buf_s.substr(166, 5);
        auto IndexBlock_offsetInfo = reinterpret_cast<OffsetInfo *>(buf + 171);
        EXPECT_EQ(_shortest_key_size, 5);
        EXPECT_EQ(_shortest_key, "key_1");
        EXPECT_EQ(IndexBlock_offsetInfo->size, 38);
        EXPECT_EQ(IndexBlock_offsetInfo->offset, 0);

        // 解析 [Footer] = 16B
        auto MetaBlock_OffsetInfo = reinterpret_cast<OffsetInfo *>(buf + 179);
        auto IndexBlock_OffsetInfo = reinterpret_cast<OffsetInfo *>(buf + 179 + 8);
        EXPECT_EQ(MetaBlock_OffsetInfo->size, 124);
        EXPECT_EQ(MetaBlock_OffsetInfo->offset, 38);
        EXPECT_EQ(IndexBlock_OffsetInfo->size, 17);
        EXPECT_EQ(IndexBlock_OffsetInfo->offset, 162);

        auto _ = system("rm -rf ./build/test_SSTableBuilder.sst");
    }
}