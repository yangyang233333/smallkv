//
// Created by qianyy on 2022/12/10.
//

#include <string>

#ifndef SMALLKV_FILTER_POLICY_H
#define SMALLKV_FILTER_POLICY_H

namespace smallkv {
    //过滤策略：主要是布隆过滤器、布谷鸟过滤器等
    class FilterPolicy {
    public:
        FilterPolicy() = default;

        //过滤器的名字
        virtual std::string policy_name() = 0;

        virtual void create_filter(const std::vector<std::string> &keys) = 0;

        // 检查是否存在，如果为false表示一定不存在，true表示可能存在
        virtual bool match() = 0;

        virtual uint64_t size() = 0;

        virtual ~FilterPolicy() = default;
    };
}

#endif //SMALLKV_FILTER_POLICY_H
