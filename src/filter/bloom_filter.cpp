//
// Created by qianyy on 2022/12/5.
//

#include "bloom_filter.h"
#include <cmath>

namespace smallkv {
//    BloomFilter::BloomFilter(int bits_per_key) : bits_per_key(bits_per_key) {
//        // 计算出最佳的哈希函数数量
//        hash_func_num = static_cast<int32_t>(0.6931471805599453 * bits_per_key);
//        // 保证哈希函数在[1,32]的范围内，防止过大或者过小
//        if (hash_func_num < 1) {
//            hash_func_num = 1;
//        }
//        if (hash_func_num > 32) {
//            hash_func_num = 32;
//        }
//    }

    BloomFilter::BloomFilter(int32_t keys_num, double false_positive) {
        // 计算出最佳的位数组大小
        int32_t bits_num = -1 * static_cast<int32_t>(std::log(false_positive) * keys_num / 0.4804530139182014);
        bits_per_key = bits_num / keys_num;
        // 计算最佳的哈希函数数量
        hash_func_num = static_cast<int32_t>(0.6931471805599453 * bits_per_key);
        // 保证哈希函数在[1,32]的范围内，防止过大或者过小
        if (hash_func_num < 1) {
            hash_func_num = 1;
        }
        if (hash_func_num > 32) {
            hash_func_num = 32;
        }
    }

    // 误差为1/10000时，存一千万条数据，布隆过滤器大小约为23MB
    // keys: 待插入的数据
    void BloomFilter::create_filter(const std::vector<std::string> &keys) {


    }

    BloomFilter::~BloomFilter() {

    }

    std::string BloomFilter::name() {
        return "BloomFilter";
    }

    uint64_t BloomFilter::size() {
        return bits_array.size();
    }


};