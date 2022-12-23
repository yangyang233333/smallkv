//
// Created by qianyy on 2022/12/5.
//
#include <iostream>
#include "utils/murmur_hash2.h"
#include "bloom_filter.h"
#include <cmath>

namespace smallkv {

    BloomFilter::BloomFilter(int32_t keys_num, double false_positive) {
        // 计算出最佳的位数组大小
        int32_t bits_num = -1 * static_cast<int32_t>(std::log(false_positive) * keys_num / 0.4804530139182014);
        bits_array.resize((bits_num + 7) / 8);
        bits_num = static_cast<int>(bits_array.size()) * 8; // 注意此处
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

    void BloomFilter::create_filter(const std::vector<std::string> &keys) {
        uint32_t bits_size = bits_array.size() * 8; // 位数组的长度
        for (const auto &key: keys) {
            uint32_t h = utils::murmur_hash2(key.c_str(), key.size());
            uint32_t delta = (h >> 17) | (h << 15); // 高17位和低15位交换
            // 模拟计算hash_func_num次哈希
            for (int j = 0; j < hash_func_num; ++j) {
                uint32_t bit_pos = h % bits_size;
                bits_array[bit_pos / 8] |= (1 << (bit_pos % 8));
                // 每轮循环h都加上一个delta，就相当于每一轮都进行了一次hash
                h += delta;
            }
        }
    }

    bool BloomFilter::exists(const std::string_view &key) {
        if (key.empty()) {
            return false;
        }
        uint32_t bits_size = bits_array.size() * 8; // 位数组的长度
        uint32_t h = utils::murmur_hash2(key.data(), key.size());
        uint32_t delta = (h >> 17) | (h << 15);
        for (int j = 0; j < hash_func_num; ++j) {
            uint32_t bit_pos = h % bits_size;
            if ((bits_array[bit_pos / 8] & (1 << (bit_pos % 8))) == 0) {
                return false;
            }
            h += delta;
        }
        return true;
    }

    uint64_t BloomFilter::size() {
        return bits_array.size();
    }

    std::string BloomFilter::policy_name() {
        return "BloomFilter";
    }
};