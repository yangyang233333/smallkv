//
// Created by qianyy on 2023/1/23.
//
#include <memory>
#include <cstdlib>
#include <utility>
#include <vector>
#include <iostream>
#include "log/log.h"
#include "memory/allocate.h"
#include "skiplist_config.h"

#ifndef SMALLKV_SKIPLIST_H
#define SMALLKV_SKIPLIST_H
namespace smallkv {
    /*
     * todo：粗略实现的一个跳表，一些地方细节不够到位。后续可以考虑认真阅读leveldb中的skiplist.h
     *
     *
     * 注：线程不安全、不支持重复的key插入
     *
     * */
    template<typename Key, typename Value>
    class SkipList {
        class Node;

    public:
        explicit SkipList(std::shared_ptr<FreeListAllocate> alloc);

        // 插入key
        void Insert(const Key &key, const Value &value);

        // 删除key
        void Delete(const Key &key);

        // 存在key则返回true
        bool Contains(const Key &key);

        // 仅用于DEBUG：打印表
        void OnlyUsedForDebugging_Print_() {
            auto p = head_->next[0];
            std::cout << "============= DEBUG =============" << std::endl;
            for (int i = 0; i < size; ++i) {
                std::cout << "key_" << i << " = " << p->key << std::endl;
                p = p->next[0];
            }
            std::cout << "============= DEBUG =============" << std::endl;
        }

        inline int GetSize() { return size; }

    private:
        int RandomLevel();

        int GetCurrentHeight();

        // 找到key节点的前缀节点，也就是找到key的待插入位置
        void FindPrevNode(const Key &key, std::vector<Node *> &prev);

        inline Node *NewNode(const Key &key, int level, const Value &value);

    private:
        Node *head_; // 头结点，高度为SkipListConfig::kMaxHeight，不存数据

        std::shared_ptr<FreeListAllocate> alloc;

        int max_level; // 当前表的最大高度节点
        int64_t size = 0; //表中数据量

        std::shared_ptr<spdlog::logger> logger = log::get_logger();
    };

    template<typename Key, typename Value>
    void SkipList<Key, Value>::Delete(const Key &key) {
        if (Contains(key) == false) {
            logger->warn("The value you want to delete does not exist. Key={}", key);
            return;
        }
        --size;

        // todo： 这里可以优化为 std::vector<Node *> prev(GetCurrentHeight, nullptr);
        //  可以减少一定的计算量，后期优化性能时考虑
        std::vector<Node *> prev(SkipListConfig::kMaxHeight, nullptr);
//        FindPrevNode(key, prev);
        int level = GetCurrentHeight() - 1;
        auto cur = head_;
        int level_of_target_node = -1;// 目标节点的层数
        while (true) {
            auto next = cur->next[level];
            if (next == nullptr) {
                if (level == 0) {
                    logger->error("A error point.");
                    break; // 遍历完成. 实际上这个分支不可能到达
                } else {
//                    prev[level] = cur;
                    --level;
                }
            } else {
                if (next->key == key) {
                    level_of_target_node = next->GetLevel();
                    prev[level] = cur;
                    --level;
                    if (level < 0) {
                        break;
                    }
                } else if (next->key < key) {
                    cur = next;
                } else if (next->key > key) {
                    prev[level] = cur;
                    --level;
                    if (level < 0) {
                        break;
                    }
                }
            }
        }
//        assert(level_of_target_node > 0);
//        assert(level_of_target_node <= prev.size());
//        logger->info("level_of_target_node={}", level_of_target_node);
        for (int i = 0; i < level_of_target_node; ++i) {
            if (prev[i] != nullptr) {
                assert(prev[i]->next[i] != nullptr);
                prev[i]->next[i] = prev[i]->next[i]->next[i];
            }
        }
    }

    template<typename Key, typename Value>
    bool SkipList<Key, Value>::Contains(const Key &key) {   // 存在key则返回true
        int level = GetCurrentHeight() - 1;
        auto cur = head_;
        while (true) {
            auto next = cur->next[level];
            if (next == nullptr) {
                if (level == 0) {
                    return false; // 已经遍历完成
                } else {
                    --level;
                }
            } else {
                if (next->key == key) {
                    return true;
                } else if (next->key < key) {
                    cur = next;
                } else if (next->key > key) {
                    if (level == 0) {
                        return false; // 只有在最后一层，遇到大于key的时候才可以认为没找到
                    } else {
                        --level; // 在非最底层遇到了大于key的数，应该下降
                    }
                }
            }
        }
    }

    template<typename Key, typename Value>
    void SkipList<Key, Value>::Insert(const Key &key, const Value &value) {
        if (Contains(key)) {
            logger->warn("A duplicate key was inserted. Key={}", key);
            return;
        }

        ++size; // 更新size

        // todo： 这里可以优化为 std::vector<Node *> prev(GetCurrentHeight, nullptr);
        //  可以减少一定的计算量，后期优化性能时考虑
        std::vector<Node *> prev(SkipListConfig::kMaxHeight, nullptr);

        // 找到key的前缀节点，并且存到prev中
        FindPrevNode(key, prev);
        int level_of_new_node = RandomLevel();
        max_level = std::max(level_of_new_node, max_level); // 更新最大高度
        auto newNode = NewNode(key, level_of_new_node, value);

        for (int i = 0; i < newNode->GetLevel(); ++i) {
            if (prev[i] == nullptr) {
                newNode->next[i] = nullptr;
                head_->next[i] = newNode;
            } else {
                newNode->next[i] = prev[i]->next[i];
                prev[i]->next[i] = newNode;
            }
        }
    }

    template<typename Key, typename Value>
    int SkipList<Key, Value>::GetCurrentHeight() {
        return max_level;
    }

    template<typename Key, typename Value>
    typename SkipList<Key, Value>::Node *SkipList<Key, Value>::NewNode(const Key &key, int level, const Value &value) {
        // todo: 不确定FreeListAllocate实现有没有问题，
        //  所以此处先使用系统allocator，稳定了再换。
        return new Node(key, level, value);
    }

    template<typename Key, typename Value>
    void SkipList<Key, Value>::FindPrevNode(
            const Key &key, std::vector<Node *> &prev) {
        int level = GetCurrentHeight() - 1;
        auto cur = head_;
        while (true) {
            auto next_node = cur->next[level];
            if (next_node == nullptr || next_node->key >= key) {
                prev[level] = cur;
                if (level > 0) {
                    --level;//遍历到了非最底层的终点，下降一层继续遍历
                } else {
                    return;
                }
            } else { // next_node != nullptr && next_node->key < key
                cur = next_node;
            }
        }
    }

    template<typename Key, typename Value>
    int SkipList<Key, Value>::RandomLevel() {
        int level = 1;
        while (level < SkipListConfig::kMaxHeight && rand() & 1) {
            ++level;
        }
        return level;
    }

    template<typename Key, typename Value>
    SkipList<Key, Value>::SkipList(std::shared_ptr<FreeListAllocate> alloc)
            :alloc(std::move(alloc)) {
        srand(time(0));
        head_ = NewNode("", SkipListConfig::kMaxHeight, "");
        max_level = 1;
        size = 0;
    }

    template<typename Key, typename Value>
    class SkipList<Key, Value>::Node {
    public:
        Node() = delete;

        Node(const Key &key, int level, const Value &value) : key(key), value(value) {
            next.resize(level, nullptr);
        }

        ~Node() = default;

        inline int GetLevel() { return next.size(); }

        const Key key;
        Value value;
        std::vector<Node *> next;
    };
}
#endif //SMALLKV_SKIPLIST_H
