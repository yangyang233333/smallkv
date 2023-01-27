//
// Created by qianyy on 2022/12/23.
//
#include <ctime>
#include <memory>
#include <gtest/gtest.h>
#include "../src/memory/allocate.h"
#include "../src/log/log.h"

namespace smallkv::unittest {

    TEST(FreeListAllocate_Allocate, basic) {
        std::unique_ptr<FreeListAllocate> allocator = std::make_unique<FreeListAllocate>();
        auto p = (int64_t *) allocator->Allocate(8);
        *p = 8;
        EXPECT_EQ(*p, 8);
        *p = 9;
        EXPECT_EQ(*p, 9);
    }

    TEST(FreeListAllocate_Deallocate, basic) {
        std::unique_ptr<FreeListAllocate> allocator = std::make_unique<FreeListAllocate>();
        auto p = (int64_t *) allocator->Allocate(8);
        *p = 8;
        allocator->Deallocate(p, 8);
    }

    TEST(FreeListAllocate_Reallocate, basic) {
        auto logger = log::get_logger();
        std::unique_ptr<FreeListAllocate> allocator = std::make_unique<FreeListAllocate>();
        auto p1 = static_cast<int32_t *>(allocator->Allocate(4));
        *p1 = 8;
        auto p2 = static_cast<int64_t *>(allocator->Reallocate(p1, 4, 8));
        *p2 = 10;
        EXPECT_EQ(*p2, 10);
    }

    TEST(FreeListAllocate, all_func) {
        auto logger = log::get_logger();
        std::unique_ptr<FreeListAllocate> allocator = std::make_unique<FreeListAllocate>();
        // 测试3000个随机长度的链表的创建和释放
        struct Node {
            Node(Node *next, int32_t data) : next(next), data(data) {}

            Node *next;
            int32_t data;
        };

        srand(time(0));
        for (int i = 0; i < 3000; ++i) { // 测试1000个链表
            Node *head = static_cast<Node *>(allocator->Allocate(sizeof(Node)));
            head->data = -1; // dummyNode
            head->next = nullptr;

            // 链表初始化
            int32_t length = rand() % 1000 + 10; // 随机一个长度作为链表长度
            auto cur = head;
            for (int j = 0; j < length; ++j) {
                cur->next = static_cast<Node *>(allocator->Allocate(sizeof(Node)));
                cur = cur->next;
                cur->data = j;
                cur->next = nullptr;
            }

            cur = head->next;
            auto prev = head;
            // 检查链表值然后释放链表内存
            for (int j = 0; j < length; ++j) {
                EXPECT_EQ(cur->data, j);
                allocator->Deallocate(prev, sizeof(Node));
                prev = cur;
                cur = cur->next;
            }
            allocator->Deallocate(prev, sizeof(Node));
        }
    }
}