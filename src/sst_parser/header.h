//
// Created by qianyy on 2023/1/30.
//
#include "db/offset_info.h"

#ifndef SMALLKV_HEADER_H
#define SMALLKV_HEADER_H
namespace smallkv {
    // SST中Header的定义
    struct Header {
        OffsetInfo MetaBlock_OffsetInfo;
        OffsetInfo IndexBlock_OffsetInfo;
    };
}
#endif //SMALLKV_HEADER_H
