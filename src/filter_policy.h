//
// Created by abc on 2022/12/6.
//
#include <string>

#ifndef SMALLKV_FILTER_POLICY_H
#define SMALLKV_FILTER_POLICY_H

namespace smallkv {
    class FilterPolicy {
    public:
        FilterPolicy() = default;

        // 过滤策略的名字
        virtual std::string name() = 0;

        virtual void create_filter() = 0;

        virtual ~FilterPolicy() = 0;
    };
}

#endif //SMALLKV_FILTER_POLICY_H
