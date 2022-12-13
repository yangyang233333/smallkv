//
// Created by qianyy on 2022/12/5.
//
#include <cstdint>

#ifndef SMALLKV_MURMUR_HASH2_H
#define SMALLKV_MURMUR_HASH2_H

namespace smallkv::utils {
    uint32_t murmur_hash2(const void *key, uint32_t len);
}

#endif //SMALLKV_MURMUR_HASH2_H
