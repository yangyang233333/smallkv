//
// Created by qianyy on 2023/1/16.
//
#include <memory>
#include <string>
#include <string_view>
#include "log/log.h"
#include "db/status.h"
#include "db/offset_info.h"

#ifndef SMALLKV_SSTABLE_BUILDER_H
#define SMALLKV_SSTABLE_BUILDER_H
namespace smallkv {
    class DataBlockBuilder;

    class FilterBlockBuilder;

    class IndexBlockBuilder;

    class FileWriter;

    class FooterBuilder;

    /*
     * SST核心结构
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
     * 参数解释：
     *      1. DataBlock_1 ~ DataBlock_N：即DataBlock，由DataBLockBuilder操作
     *      2. MetaBlock：存放Filter等信息，这里每个SST只设置一个MetaBlock
     *      3. IndexBlock_1 ~ IndexBlock_N：存放对应的DataBLock的Offset信息、最大Key信息
     *      4. Footer：存放MetaBlock、IndexBlock的Offset信息
     *
     * */
    class SSTableBuilder final {
    private:
        std::shared_ptr<DataBlockBuilder> dataBlockBuilder = nullptr;    // 操作DataBlock
        std::shared_ptr<IndexBlockBuilder> indexBlockBuilder = nullptr;  // 操作IndexBlock
        std::shared_ptr<FilterBlockBuilder> filterBlockBuilder = nullptr;// 操作Filter
        std::shared_ptr<FileWriter> fileWriter = nullptr;                // 操作SST的落盘
        std::shared_ptr<FooterBuilder> footerBuilder = nullptr;          // 操作Footer

        std::shared_ptr<spdlog::logger> logger; // logger

        // 分别保存FilterBlock、IndexBlock的offset信息，然后保存在Footer中
        OffsetInfo FilterBlock_offset{0, 0};
        OffsetInfo IndexBlock_offset{0, 0};

        // Filter中所需要的信息
        int32_t _keys_num = 0;
        double _false_positive = 0;

        std::string index_data; // 暂时保存index_block数据

        int32_t key_count = 0; // 对已经插入的key计数

        // 记录上一个add的key。 此处使用string_view会失效，可能有问题
        std::string pre_key;
        // 前一个DataBlock的offset信息
        OffsetInfo pre_offset_info{};

    public:
        // keys_num表示这个SST中Key的预期数量（全部DataBlock中Key的预期数量），超过了会导致Filter误判率升高，影响查找效率
        // false_positive表示假阳性率
        explicit SSTableBuilder(int32_t keys_num, std::shared_ptr<FileWriter> _fileWriter,
                                double false_positive = 0.01);

        ~SSTableBuilder() = default;

        // 插入KV对
        DBStatus add(const std::string &key, const std::string &val);

        // 最后一个Data Block存在数据未刷盘，所以在整个SST写完的时候，需要显示调用finish_sst
        // 调用了finish_sst表示本sst已经完整写到磁盘了
        DBStatus finish_sst();

    private:
        /*
         * 找到能key1和key2的最短不同前缀，并且key1< ret < key2
         * e.g.: key1="abcd", key2="abecd"
         * 则可以返回ret="abd"
         *
         * */
        static std::string find_shortest_key(const std::string &key1, const std::string &key2);
    };
}
#endif //SMALLKV_SSTABLE_BUILDER_H







































