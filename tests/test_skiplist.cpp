//
// Created by qianyy on 2023/1/4.
//
#include <iostream>
#include <ctime>
#include <memory>
#include <string>
#include <gtest/gtest.h>
#include "memtable/skiplist.h"
#include "memory/allocate.h"

namespace smallkv::unittest {
    TEST(skiplist, Insert) {
        auto alloc = std::make_shared<FreeListAllocate>();
        std::shared_ptr<SkipList<std::string, std::string>> skiplist =
                std::make_shared<SkipList<std::string, std::string>>(alloc);
        skiplist->Insert("1", "value_1");
        skiplist->Insert("2", "value_2");
    }

    TEST(skiplist, Insert2) {
        auto alloc = std::make_shared<FreeListAllocate>();
        std::shared_ptr<SkipList<std::string, std::string>> skiplist =
                std::make_shared<SkipList<std::string, std::string>>(alloc);
        for (int i = 0; i < 100; ++i) {
            skiplist->Insert(std::to_string(i), "value_" + std::to_string(i));
        }
    }

    TEST(skiplist, Contains) {
        auto alloc = std::make_shared<FreeListAllocate>();
        std::shared_ptr<SkipList<std::string, std::string>> skiplist =
                std::make_shared<SkipList<std::string, std::string>>(alloc);

        skiplist->Insert("1", "value_1");
        skiplist->Insert("3", "value_3");
        skiplist->Insert("5", "value_5");
        EXPECT_EQ(skiplist->Contains("1"), true);
        EXPECT_EQ(skiplist->Contains("3"), true);
        EXPECT_EQ(skiplist->Contains("5"), true);
        EXPECT_EQ(skiplist->Contains("0"), false);
        EXPECT_EQ(skiplist->Contains("2"), false);
        EXPECT_EQ(skiplist->Contains("4"), false);
        EXPECT_EQ(skiplist->Contains("6"), false);
    }

    TEST(skiplist, Contains2) {
        auto alloc = std::make_shared<FreeListAllocate>();
        std::shared_ptr<SkipList<std::string, std::string>> skiplist =
                std::make_shared<SkipList<std::string, std::string>>(alloc);

        const int N = 2000;
        srand(time(0));
        std::vector<std::string> yes;
        std::vector<std::string> no;
        for (int i = 0; i < N; ++i) {
            if (rand() & 1) {
                yes.push_back(std::to_string(i));
            } else {
                no.push_back(std::to_string(i));
            }
        }
        for (const auto &ye: yes) {
            skiplist->Insert(ye, "value_" + ye);
        }

        for (const auto &y: yes) {
            EXPECT_EQ(skiplist->Contains(y), true);
        }
        for (const auto &n: no) {
            EXPECT_EQ(skiplist->Contains(n), false);
        }
    }

    TEST(skiplist, Delete) {
        auto alloc = std::make_shared<FreeListAllocate>();
        std::shared_ptr<SkipList<std::string, std::string>> skiplist =
                std::make_shared<SkipList<std::string, std::string>>(alloc);

        skiplist->Insert("1", "value_1");
        skiplist->Insert("3", "value_3");
        skiplist->Insert("5", "value_5");
        EXPECT_EQ(skiplist->Contains("1"), true);
        EXPECT_EQ(skiplist->Contains("3"), true);
        EXPECT_EQ(skiplist->Contains("5"), true);

        skiplist->Delete("1");
        skiplist->Delete("3");
        skiplist->Delete("5");
        EXPECT_EQ(skiplist->Contains("1"), false);
        EXPECT_EQ(skiplist->Contains("3"), false);
        EXPECT_EQ(skiplist->Contains("5"), false);
    }

    TEST(skiplist, Delete2) {
        auto alloc = std::make_shared<FreeListAllocate>();
        std::shared_ptr<SkipList<std::string, std::string>> skiplist =
                std::make_shared<SkipList<std::string, std::string>>(alloc);
        const int N = 2000;
        for (int i = 0; i < N; ++i) {
            skiplist->Insert(std::to_string(i), "value_" + std::to_string(i));
            int flag = rand() & 0x1;
            if (flag == 1) {
                skiplist->Delete(std::to_string(i));
                EXPECT_EQ(skiplist->Contains(std::to_string(i)), false);
            } else {
                EXPECT_EQ(skiplist->Contains(std::to_string(i)), true);
            }
        }
    }

    TEST(skiplist, Get) {
        auto alloc = std::make_shared<FreeListAllocate>();
        std::shared_ptr<SkipList<std::string, std::string>> skiplist =
                std::make_shared<SkipList<std::string, std::string>>(alloc);

        skiplist->Insert("1", "value_1");
        skiplist->Insert("3", "value_3");
        skiplist->Insert("5", "value_5");

        EXPECT_EQ(skiplist->Get("0"), std::nullopt);
        EXPECT_EQ(skiplist->Get("1"), "value_1");
        EXPECT_EQ(skiplist->Get("2"), std::nullopt);
        EXPECT_EQ(skiplist->Get("3"), "value_3");
        EXPECT_EQ(skiplist->Get("4"), std::nullopt);
        EXPECT_EQ(skiplist->Get("5"), "value_5");
    }

    TEST(skiplist, Get2) {
        auto alloc = std::make_shared<FreeListAllocate>();
        std::shared_ptr<SkipList<std::string, std::string>> skiplist =
                std::make_shared<SkipList<std::string, std::string>>(alloc);
        const int N = 1234;
        for (int i = 0; i < N; ++i) {
            skiplist->Insert(std::to_string(i), "value_" + std::to_string(i));
            if (i & 1) {
                skiplist->Delete(std::to_string(i));
            }
        }
        for (int i = 0; i < N; ++i) {
            if (i & 1) {
                EXPECT_EQ(skiplist->Get(std::to_string(i)), std::nullopt);
            } else {
                EXPECT_EQ(skiplist->Get(std::to_string(i)), "value_" + std::to_string(i));
            }
        }
    }
}
