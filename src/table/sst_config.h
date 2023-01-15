//
// Created by qianyy on 2023/1/13.
//
#include <cstdint>

#ifndef SMALLKV_SST_CONFIG_H
#define SMALLKV_SST_CONFIG_H
namespace smallkv {
    // SST Config信息
    struct SSTConfigInfo {
        SSTConfigInfo() = delete; // 禁止创建对象

        ~SSTConfigInfo() = delete;

        // 每16个Record生成一个Restart Point
        // 这16个Record称为一个Record Group
        static constexpr int32_t RESTART_INTERVAL = 16;

        // 这里指的是Data Block中所有Record总和的最大大小，超过后就应该重新生成一个DataBlock
        static constexpr int32_t MAX_DATA_BLOCK_SIZE = 4096; // 4K
    };
}
#endif //SMALLKV_SST_CONFIG_H
