//
// Created by qianyy on 2023/1/19.
//
#include <string>
#include "db/offset_info.h"
#include "db/status.h"

#ifndef SMALLKV_FOOTER_H
#define SMALLKV_FOOTER_H
namespace smallkv {
    /*
     * SSTable中的Footer模块
     * _data数据schema如下所示：
     *         +---------------------------+----------------------------+
     *         | MetaBlock_OffsetInfo (8B) | IndexBlock_OffsetInfo (8B) |
     *         +---------------------------+----------------------------+
     * MetaBlock_OffsetInfo: 存放MetaBlock的Offset信息
     * IndexBlock_OffsetInfo：存放IndexBlock的offset信息
     *
     * */
    class FooterBuilder final {
    private:
//        OffsetInfo meta_block_offset_info;
//        OffsetInfo index_block_offset_info;
        std::string _data;

    public:
        DBStatus add(const OffsetInfo &meta_block_offset_info,
                     const OffsetInfo &index_block_offset_info);

        inline void clear() {
            _data.clear();
        }

        inline std::string_view data() { return _data; }
    };
}
#endif //SMALLKV_FOOTER_H
