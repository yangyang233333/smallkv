//
// Created by qianyy on 2022/12/5.
//
#include <cstdint>
#include <string>
#include <vector>
#include "filter_policy.h"

#ifndef SMALLKV_BLOOM_FILTER_H
#define SMALLKV_BLOOM_FILTER_H

namespace smallkv {
    class BloomFilter final : public FilterPolicy {
    public:
        BloomFilter() = delete;

        // bits_per_key指每个key所占据的位数
//        explicit BloomFilter(int32_t bits_per_key);

        // keys_num：过滤器中的元素数量
        // false_positive：假阳性概率
        BloomFilter(int32_t keys_num, double false_positive);

        // 输出过滤器的名字，当前是布隆过滤器
        static std::string name();

        uint64_t size() override;

        // 创建过滤器
        void create_filter(const std::vector<std::string> &keys) override;

        ~BloomFilter() override;

    private:
        // 哈希函数的个数
        int32_t hash_func_num = 0;
        // 每个key所占据的位数
        int32_t bits_per_key = 0;
        // bit数组
        std::vector<uint8_t> bits_array;
    };
}

#endif //SMALLKV_BLOOM_FILTER_H
