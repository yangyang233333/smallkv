//
// Created by qianyy on 2023/2/4.
//
#include "db/offset_info.h"
#include <string>

#ifndef SMALLKV_RESTART_POINT_H
#define SMALLKV_RESTART_POINT_H
namespace smallkv {
    struct RestartPoint {
        int record_num = 0;
        OffsetInfo rp_offset;
        std::string fullkey;
    };
}
#endif //SMALLKV_RESTART_POINT_H
