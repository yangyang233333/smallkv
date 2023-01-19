//
// Created by qianyy on 2023/1/13.
//
#include <cstdint>

#ifndef SMALLKV_OFFSET_INFO_H
#define SMALLKV_OFFSET_INFO_H

namespace smallkv {

    // 主要用于SSTable中的offset信息
    struct OffsetInfo {
        int32_t size = 0; // 偏移量
        int32_t offset = 0; // 大小

        // 计算一个offsetInfo实际占用的存储空间
        int32_t get_offset_info_size() {
            return sizeof(size) + sizeof(offset);
        }

        void clear() {
            size = 0;
            offset = 0;
        }
    };
}
#endif //SMALLKV_OFFSET_INFO_H
