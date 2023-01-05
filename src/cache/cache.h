//
// Created by qianyy on 2023/1/4.
//
#include <vector>
#include <cstdint>
#include <string>
#include <memory>

#include "cache_policy.h"
#include "lru.h"

#ifndef SMALLKV_CACHE_H
#define SMALLKV_CACHE_H

namespace smallkv {
    template<typename K, typename V>
    class Cache {
    private:
        // 设置5个分片，也就是5个LRU Holder。一定程度上可以减少碰撞
        // 此外分片还可以减少锁的粒度(将锁的范围减少到原来的1/SHARDING_NUM)，提高了并发性
        static constexpr uint64_t SHARDING_NUM = 5;
        std::vector<std::shared_ptr<CachePolicy<K, V>>> caches;

        static constexpr std::hash<K> hash_fn{};
    public:
        explicit Cache(uint32_t cap) {
            caches.resize(SHARDING_NUM);
            for (int i = 0; i < SHARDING_NUM; ++i) {
                caches[i] = std::make_shared<LRU<K, V, MutexLock>>(cap);
            }
        }

        ~Cache() = default;

        // 增
        void insert(const K &key, V *val) {
            uint64_t sharding_index = hash_fn(key) % SHARDING_NUM;
            caches[sharding_index]->insert(key, val);
        }

        // 删
        void erase(const K &key) {
            uint64_t sharding_index = hash_fn(key) % SHARDING_NUM;
            caches[sharding_index]->erase(key);
        }

        // 查
        Node<K, V> *get(const K &key) {
            uint64_t sharding_index = hash_fn(key) % SHARDING_NUM;
            return caches[sharding_index]->get(key);
        }

        // 释放节点(引用计数减一)
        void release(const K &key) {
            uint64_t sharding_index = hash_fn(key) % SHARDING_NUM;
            caches[sharding_index]->release(key);
        }

        // 销毁节点的回调函数
        void register_clean_handle(std::function<void(const K &key, V *val)> destructor) {
            for (int i = 0; i < SHARDING_NUM; ++i) {
                caches[i]->register_clean_handle(destructor);
            }
        }
    };
}

#endif //SMALLKV_CACHE_H
