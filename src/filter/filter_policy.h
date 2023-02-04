//
// Created by qianyy on 2022/12/10.
//

#include <string>
#include <string_view>

#ifndef SMALLKV_FILTER_POLICY_H
#define SMALLKV_FILTER_POLICY_H

namespace smallkv {
    //过滤策略：主要是布隆过滤器、布谷鸟过滤器等
    class FilterPolicy {
    public:
        FilterPolicy() = default;

        virtual ~FilterPolicy() = default;

        // 返回数据，用于持久化
        virtual std::string &data() = 0;

        //过滤器的名字
        virtual std::string policy_name() = 0;

        // 返回hash函数的数量
        virtual int32_t get_hash_num() = 0;

        virtual void create_filter(const std::vector<std::string> &keys) = 0;

        // 主要用于读取sst的时候创建filter
        virtual void create_filter2(int32_t hash_func_num_,
                                    std::string &bits_array_) = 0;

        // 检查是否存在，如果为false表示一定不存在，true表示可能存在
        virtual bool exists(const std::string_view &key) = 0;

        virtual uint64_t size() = 0;
    };
}

#endif //SMALLKV_FILTER_POLICY_H
