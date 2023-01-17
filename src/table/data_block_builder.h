//
// Created by qianyy on 2023/1/13.
//
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>
#include "../db/status.h"
#include "../db/offset_info.h"

#ifndef SMALLKV_DATA_BLOCK_Builder_H
#define SMALLKV_DATA_BLOCK_Builder_H
namespace smallkv {
    /*
     * DataBlock包含三个部分，分别是Records、Restart Points、Restart Point Count
     * 数据排布如下:
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
     * 1. [Record_1 ~ Record_N] Records包含N个Record，其中单个Record的schema如下所示：
     *     Record的schema：
     *           +--------------------+----------------------+---------------+----------------------+---------------+
     *           | shared_key_len(4B) | unshared_key_len(4B) | value_len(4B) | unshared_key_content | value_content |
     *           +--------------------+----------------------+---------------+----------------------+---------------+
     *     Record的大小(单位: B) = 4 + 4 + 4 + unshared_key_len + value_len
     *     参数的解释：
     *          shared_key_len: 本条 Record 和对应Restart Point的fullkey的共享头的长度
     *          其他参数意义不言自明
     *
     * 2. [Restart_1 ~ Restart_K] Restart Points
     *     DataBlock中的重启点，保存对应的 Record Group 的OffsetInfo【据此可以分析出第一个key】，用于二分查找
     *
     *     Restart_Point的schema如下：
     *            +----------------+----------------+
     *            | record_num(4B) | OffsetInfo(8B) |
     *            +----------------+----------------+
     *      参数的解释：
     *          record_num: 重启点对应的 Record Group 中的 Record数量
     *          OffsetInfo: 重启点对应的 Record Group 的size和offset
     *
     * 3. [Restart_Num、Restart_Offset]
     *      意义不言自明
     *
     *
     * */
    class DataBlockBuilder final {
        using size_type = std::string::size_type;
    private:
        std::vector<int32_t> record_group_offset; // 暂时保存一下每个record_group的offset
        std::string pre_key;    // 当前key的前一个key
        std::string _data;      // data block的所有数据
        int32_t record_num = 0; // 当前已经写入的Record的数量

    public:
        DataBlockBuilder() = default;

        ~DataBlockBuilder() = default;

        // 添加一个Key-Value对
        DBStatus add(const std::string_view &key, const std::string_view &value);

        // 当Block中的所有Record的大小超过阈值时，停止写入，并且生成重启点等信息
        DBStatus add_restart_points();

        std::string_view data() { return _data; }

        // 清空DataBlock中的所有数据
        void reset();

    private:
        // 获得本DataBlock中已写入的所有Record的size
        size_type get_records_size() { return _data.size(); }
    };
}
#endif //SMALLKV_DATA_BLOCK_Builder_H
