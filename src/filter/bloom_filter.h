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

//        bits_per_key指每个key所占据的位数
//        explicit BloomFilter(int32_t bits_per_key);

        // keys_num：预期的键数量（要设大一点，防止超出导致过滤器失效）
        // false_positive：假阳性概率
        BloomFilter(int32_t keys_num, double false_positive);

        // 输出过滤器的名字，当前是布隆过滤器
        std::string policy_name() override;


        uint64_t size() override;

        // 创建过滤器
        // 误差为1/10000时，存一千万条数据，布隆过滤器大小约为23MB
        // keys: 待插入的数据
        void create_filter(const std::vector<std::string> &keys) override;

        // 判断元素key是否存在
        // 返回值： false=不存在；true=可能存在；
        bool exists(const std::string_view &key) override;

        ~BloomFilter() override = default;

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
