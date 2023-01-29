//
// Created by qianyy on 2023/1/23.
//
#include <memory>
#include <vector>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <optional>
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

        // 插入一个新的key
        void Insert(const Key &key, const Value &value);

        // 删除key
        void Delete(const Key &key);

        // 存在key则返回true
        bool Contains(const Key &key);

        // 注：如果要找的key不存在，则返回nullopt
        std::optional<Value> Get(const Key &key);

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

        inline int64_t GetMemUsage() { return mem_usage; }

        // 迭代skiplist，主要是给MemTable中的MemeIterator调用
        class SkipListIterator {
        public:
            explicit SkipListIterator(const SkipList *list);

            // 如果当前iter指向的位置有效，则返回true
            bool Valid();

            const Key &key();

            const Value &value();

            void Next();

            // todo: Prev暂时不支持，需要修改底层的跳变api，后续有空再说
            void Prev() = delete;

            // 将当前node移到表头
            // 必须要先调用此函数才可以进行迭代
            void MoveToFirst();

        private:
            const SkipList *list_;
            Node *node; // 当前iter指向的节点
        };

    private:
        int RandomLevel();

        int GetCurrentHeight();

        // 找到key节点的前缀节点，也就是找到key的待插入位置
        void FindPrevNode(const Key &key, std::vector<Node *> &prev);

        inline Node *NewNode(const Key &key, int level, const Value &value);

    private:
        Node *head_; // 头结点，高度为SkipListConfig::kMaxHeight，不存数据

        std::shared_ptr<FreeListAllocate> alloc;

        int max_level;         // 当前表的最大高度节点
        int64_t size = 0;      // 表中数据量(kv键值对数量)
        int64_t mem_usage = 0; // kv键值对所占用的内存大小，单位：Byte

        std::shared_ptr<spdlog::logger> logger = log::get_logger();
    };

    template<typename Key, typename Value>
    void SkipList<Key, Value>::SkipListIterator::MoveToFirst() {
        node = list_->head_->next[0];
    }

    template<typename Key, typename Value>
    void SkipList<Key, Value>::SkipListIterator::Next() {
        assert(Valid());
        node = node->next[0]; // 遍历肯定是在跳表最底层进行遍历，所以是0
    }

    template<typename Key, typename Value>
    const Key &SkipList<Key, Value>::SkipListIterator::key() {
        assert(Valid());
        return node->key;
    }

    template<typename Key, typename Value>
    const Value &SkipList<Key, Value>::SkipListIterator::value() {
        assert(Valid());
        return node->value;
    }

    template<typename Key, typename Value>
    bool SkipList<Key, Value>::SkipListIterator::Valid() {
        return node != nullptr;
    }

    template<typename Key, typename Value>
    SkipList<Key, Value>::SkipListIterator::SkipListIterator(const SkipList *list) : list_(list) {
        node = nullptr;
    }

    template<typename Key, typename Value>
    std::optional<Value> SkipList<Key, Value>::Get(const Key &key) {
        int level = GetCurrentHeight() - 1;
        auto cur = head_;
        while (true) {
            auto next = cur->next[level];
            if (next == nullptr) {
                if (level == 0) {
                    // 遍历到这里说明key不存在
                    return std::nullopt;
                } else {
                    --level;
                }
            } else {
                if (next->key == key) {
                    return next->value; // 找到了
                } else if (next->key < key) {
                    cur = next;
                } else if (next->key > key) {
                    if (level == 0) {
                        // 遍历到这里说明key不存在
                        return std::nullopt;
                    } else {
                        --level; // 在非最底层遇到了大于key的数，应该下降
                    }
                }
            }
        }
    }

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

        // 更新内存占用
        mem_usage -= key.size();
        mem_usage -= prev[0]->next[0]->value.size(); // prev[0]->next[0]指向待删除的节点

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

        // todo：这种写法导致了Key、Value必须为string、string_view类型，
        //  模板名存实亡，后续需要改进。
        mem_usage += key.size();
        mem_usage += value.size();

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
