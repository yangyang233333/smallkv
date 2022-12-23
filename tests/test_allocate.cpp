//
// Created by qianyy on 2022/12/23.
//
#include <ctime>
#include <memory>
#include <gtest/gtest.h>
#include "../src/memory/allocate.h"
#include "../src/log/log.h"

namespace smallkv {


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

    TEST(FreeListAllocate_ALL, basic) {
        auto logger = log::get_logger();
        std::unique_ptr<FreeListAllocate> allocator = std::make_unique<FreeListAllocate>();
        // 测试一个链表(长度为10~1000)
        struct Node {
            Node(Node *next, int32_t data) : next(next), data(data) {}

            Node *next;
            int32_t data;
        };
        Node *head = static_cast<Node *>(allocator->Allocate(sizeof(Node)));
        head->data = -1; // dummyNode
        head->next = nullptr;
        srand(time(0));
        for (int i = 0; i < 1000; ++i) { // 测试1000个链表
            int32_t length = rand() % 1000 + 10; // 随机一个长度作为链表长度
            auto p = head;
            for (int j = 0; j < length; ++j) {
                p->next = static_cast<Node *>(allocator->Allocate(sizeof(Node)));
                p->next->data = j;
                p->next->next = nullptr;
                p = p->next;
            }
            p = head;
            auto prev = p;
            for (int j = 0; j < length; ++j) {
                p = p->next;
                EXPECT_EQ(p->data, j);
//                allocator->Deallocate(prev, sizeof(Node));
//                prev = prev->next;
            }
        }

    }
}