//
// Created by qianyy on 2023/1/31.
//
#include <stdint.h>
#include <string>
#include "db/offset_info.h"

#ifndef SMALLKV_INDEX_BLOCK_H
#define SMALLKV_INDEX_BLOCK_H
namespace smallkv {
    struct IndexBlock {
        std::string _shortest_key; // 保证 _shortest_key >= 对应的DataBlock中的最大key
        OffsetInfo offsetInfo;     // 对应DataBlock的offset信息
    };
}
#endif //SMALLKV_INDEX_BLOCK_H
