//
// Created by qianyy on 2023/1/4.
//
#include <cstdint>
#include "node.h"
#include <functional>

#ifndef SMALLKV_CACHE_POLICY_H
#define SMALLKV_CACHE_POLICY_H

namespace smallkv {
    template<typename K, typename V>
    class CachePolicy {
    public:
        CachePolicy() = default;

        virtual ~CachePolicy() = default;

        // 增
        virtual void insert(const K &key, V *val) = 0;

        // 删
        virtual void erase(const K &key) = 0;

        // 查
        virtual Node<K, V> *get(const K &key) = 0;

        // 存在则返回true
        virtual bool contains(const K &key) = 0;

        // 释放节点(引用计数减一)
        virtual void release(const K &key) = 0;

        // 注入销毁节点的回调函数
        virtual void register_clean_handle(std::function<void(const K &key, V *val)> destructor) = 0;
    };
}
#endif //SMALLKV_CACHE_POLICY_H
