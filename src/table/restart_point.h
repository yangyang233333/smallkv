//
// Created by qianyy on 2023/1/13.
//
#include "../db/offset_info.h"
#include "sst_config.h"

#ifndef SMALLKV_RESTART_POINT_H
#define SMALLKV_RESTART_POINT_H
namespace smallkv {
    // DataBlock中的重启点，保存全量的key，用于二分查找
    struct [[deprecated]] RestartPoint {
        // 保存重启点对应的Record Group中的第一个KEY(完整KEY)，也就是字典序最小的KEY
        const char *full_key = nullptr;
        int32_t full_key_size = 0;

        // 保存重启点对应的Record Group中的record数量
        int32_t record_num = 0;

        // 保存重启点对应的Record Group的offset信息
        OffsetInfo offsetInfo{};

        // 计算一个RestartPoint所占用的实际存储空间
        int32_t get_restart_point_size() {
            return full_key_size + sizeof(full_key_size) + sizeof(record_num) + offsetInfo.get_offset_info_size();
        }
    };
}
#endif //SMALLKV_RESTART_POINT_H
