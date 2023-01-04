//
// Created by qianyy on 2023/1/4.
//
#include "stdint.h"

#ifndef SMALLKV_NODE_H
#define SMALLKV_NODE_H
namespace smallkv {
    template<typename K, typename V>
    class Node {
    public:
        Node() = default;

        K key; // 只深复制Key。实际上对于某些大Key，性能也很差，有待改进
        V *val;

        int32_t ref_cnt = 0;  // 引用计数。这里使用无符号可能会有些问题
        uint32_t hash_val = 0;

        bool in_cache = false; // 默认不在缓存中
        uint64_t last_access_time = 0; // 最近访问时间
    };
}
#endif //SMALLKV_NODE_H
