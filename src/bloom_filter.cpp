//
// Created by abc on 2022/12/5.
//

#include "bloom_filter.h"

namespace smallkv {
    BloomFilter::BloomFilter(int bits_per_key) {
        k = (std::int32_t) (bits_per_key * 0.7);
        if (k < 1) {
            k = 1;
        }
        if (k > 30) {
            k = 30;
        }
    }

    void BloomFilter::create_filter() {

    }

    BloomFilter::~BloomFilter() {

    }
};