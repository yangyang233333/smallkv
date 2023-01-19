//
// Created by qianyy on 2023/1/18.
//
#include <memory>
#include <string>
#include <utility>
#include "../db/offset_info.h"
#include "../db/status.h"

#ifndef SMALLKV_INDEX_BLOCK_BUILDER_H
#define SMALLKV_INDEX_BLOCK_BUILDER_H
namespace smallkv {
    class FileWriter;

    class IndexBlockBuilder final {
    private:
        /*
         * std::string _shortest_key: 保证：shortest_key = min{shortest_key > 对应的DataBlock的最大key}
         * OffsetInfo _offsetInfo{}: 保存对应DataBlock的Offset信息
         * _data的数据排布如下：
         *         +------------------------+---------------+-----------------+
         *         | _shortest_key_size(4B) | _shortest_key | _offsetInfo(8B) |
         *         +------------------------+---------------+-----------------+
         *
         * */
        // 用来序列化_shortest_key、_offsetInfo
        std::string _data;

    public:
        IndexBlockBuilder() = default;

        ~IndexBlockBuilder() = default;

        DBStatus add(const std::string &shortest_key,
                     const OffsetInfo &offsetInfo);

        // 返回_data
        inline std::string data() { return _data; }

        // 清除所有字段
        inline void clear() {
            _data.clear();
        }
    };
}
#endif //SMALLKV_INDEX_BLOCK_BUILDER_H
