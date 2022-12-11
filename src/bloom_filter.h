//
// Created by abc on 2022/12/5.
//
#include <cstdint>
#include <string>
#include "filter_policy.h"

#ifndef SMALLKV_BLOOM_FILTER_H
#define SMALLKV_BLOOM_FILTER_H

namespace smallkv {
    class BloomFilter final : public FilterPolicy {
    public:
        BloomFilter() = default;

        explicit BloomFilter(int32_t bits_per_key);

        void create_filter() override;

        ~BloomFilter() override;

    private:
        //哈希函数的个数，也就是0.7*位数组长度/元素个数 = 0.7*bits_per_key
        int32_t k{};
    };
}

#endif //SMALLKV_BLOOM_FILTER_H
