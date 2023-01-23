//
// Created by qianyy on 2023/1/22.
//
#include <cstdint>

#ifndef SMALLKV_LOG_FORMAT_H
#define SMALLKV_LOG_FORMAT_H
namespace smallkv {

    struct WALConfig {
        WALConfig() = delete;

        ~WALConfig() = delete;

        static constexpr int32_t kHeaderSize = 8;

    };
}
#endif //SMALLKV_LOG_FORMAT_H
