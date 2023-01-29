//
// Created by qianyy on 2023/1/4.
//
#include <list>
#include <unordered_map>
#include <cassert>

#include "cache_policy.h"
#include "../utils/lock.h"
#include "../log/log.h"

#ifndef SMALLKV_LRU_H
#define SMALLKV_LRU_H

namespace smallkv {
    /*
     * 对typename LockType = NullLock的解释：
     * 这里LockType是锁的类型，默认情况为NullLock即为无锁，
     * 可以传入三种锁：NullLock、MutexLock、SpinLock
     *
     * 为什么不在运行时判断？例如if (LockType == "mutex_lock") lock.
     * 答：运行时if存在开销。
     * */
    template<typename K, typename V, typename LockType = NullLock>
    class LRU final : public CachePolicy<K, V> {
    private:
        const uint32_t cap = 0; // 最大容量

        // 存放所有node的指针
        std::list<Node<K, V> *> nodes;
        // 保存从key到node*的映射
        std::unordered_map<K, typename std::list<Node<K, V> *>::iterator> index;

        // 回收站，或者称为待删除列表
        // 这里的node已经不在缓存中，但是部分节点的引用计数还没有降为0（有上层在使用），
        // 所以不能立即删除，需要开启一个线程定时检查，进行清除。
        std::unordered_map<K, Node<K, V> *> garbage_station;

        // 销毁节点的回调函数
        std::function<void(const K &key, V *val)> destructor;

        LockType locker; // 锁

        static constexpr std::hash<K> hash_fn{}; // 计算key的哈希

    private:
        /*
         * (虚幻的)删除节点node的步骤：(这里并不是真的erase节点，而是将node移动到回收站暂存)
         * 0. 从nodes和index中删除
         * 1. 标记node不在缓存中
         * 2. 将node移动到待删除队列
         * 3. node引用计数减一
         *
         * 解释一下为什么需要引用计数？
         * 实际上这里存在两种策略：第一种是一旦LRU满了，就直接删除node，不需要什么引用计数，
         * 但是这种方法性能较低，因为每次查找Get都是直接返回一个node的深拷贝；第二种是采用
         * 引用计数的方法，Get直接返回一个node指针，但是这样的话，如果LRU满了，就不能直接
         * 删除node（此时引用计数不一定为0，表示被上层使用），需要把它暂时移动到回收站（指针移动），
         * 等待其引用计数降为0，才可以删除。
         * */
        void phantom_erase_node(Node<K, V> *node) {
            if (node) {
                node->in_cache = false;
                nodes.erase(index[node->key]);
                index.erase(node->key);
                garbage_station[node->key] = node; // 移动到待删除队列
                unref(node);
            }
        }

        // 引用计数+1
        void ref(Node<K, V> *node) {
            if (node) {
                ++node->ref_cnt;
            }
        }

        // 引用计数-1
        void unref(Node<K, V> *node) {
            if (node) {
                --node->ref_cnt;
                if (node->ref_cnt <= 0) {
                    destructor(node->key, node->val);
                    garbage_station.erase(node->key); // 真正的删除
                    delete node;
                    node = nullptr;
                }
            }
        }

    public:
        explicit LRU(uint32_t cap) : cap(cap) {}

        ~LRU() {
            // todo: 此处调用phantom_erase_node会出现double free or corruption (out)，暂时不知道原因
            // todo: 不析构也行，此处LRU的析构表示程序结束，此时内存会被系统回收
//            for (auto &node: nodes) {
//                // node被标记为0后，在phantom_erase_node中的unref函数中会被清除
//                node->ref_cnt = 0;
//                phantom_erase_node(node);
//            }
        }

        // 增
        void insert(const K &key, V *val) override {
            ScopedLock<LockType> lock_guard(locker);
//            log::get_logger()->info("nodes_size={}", nodes.size());
//            log::get_logger()->info("[insert] was called, arg: key={}, *val={}", key, *val);

            // node init
            auto new_node = new Node<K, V>();
            new_node->key = key;
            new_node->val = val;
            new_node->ref_cnt = 1; // 引用计数初始化为1
            new_node->hash_val = hash_fn(key);
            new_node->in_cache = true;
            new_node->last_access_time = time(0); // 使用当前时间作为last_access_time

            auto f = index.find(new_node->key);
            if (f == index.end()) { // 当前节点是一个新的节点
                nodes.push_front(new_node);
                index[new_node->key] = nodes.begin();
                if (nodes.size() > cap) { // 满了就淘汰最后的节点
                    auto old_node = nodes.back();
                    // 删除节点(实际上并不是真的删除，只是从LRU中移出到待删除队列中，直到ref_cnt为0才会真正时删除)
                    phantom_erase_node(old_node);
                }
            } else {
                // 当前节点存在，说明发生了更新，需要移动到队首
                nodes.splice(nodes.begin(), nodes, index[new_node->key]);
                index[new_node->key] = nodes.begin(); // 需要更新索引
            }
        }

        // 删
        void erase(const K &key) override {
            ScopedLock<LockType> lock_guard(locker);
//            log::get_logger()->info("[erase] was called, arg: key={}", key);
            auto iter = index.find(key);
            if (iter != index.end()) {
                phantom_erase_node(*(iter->second));// 从LRU的缓存中删除，然后放到回收站
            }
        }

        // 查
        Node<K, V> *get(const K &key) override {
            ScopedLock<LockType> lock_guard(locker);
            //log::get_logger()->info("[get] was called, arg: key={}", key);
            auto iter = index.find(key);
            if (iter == index.end()) {
                return nullptr;
            }
            nodes.splice(nodes.begin(), nodes, iter->second); // 移动到头部
            index[key] = nodes.begin();
            ref(*(iter->second));
            return *(iter->second);
        }

        // 存在则返回true
        bool contains(const K &key) {
            ScopedLock<LockType> lock_guard(locker);
            return index.find(key) != index.end();
        }

        // 释放节点(引用计数减一)
        void release(const K &key) override {
            ScopedLock<LockType> lock_guard(locker);
            //log::get_logger()->info("[release] was called, arg: key={}", key);
            unref(*(index[key]));
        }

        // 注入销毁节点的回调函数
        void register_clean_handle(std::function<void(const K &key, V *val)> _destructor) override {
            this->destructor = _destructor;
        }
    };

}

#endif //SMALLKV_LRU_H
