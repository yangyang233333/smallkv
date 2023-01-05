//
// Created by qianyy on 2023/1/4.
//
#include <time.h>
#include <memory>
#include <string>
#include <gtest/gtest.h>
#include "../src/cache/lru.h"
#include "../src/cache/node.h"
#include "../src/cache/cache.h"
#include "../src/cache/cache_policy.h"

namespace smallkv {
    TEST(lru_cache, insert) {
        auto cache_holder = std::make_unique<Cache<std::string, std::string>>(5000);

        cache_holder->register_clean_handle([](const std::string &key, std::string *val) {
            delete val;
        });
        for (int i = 0; i < 5000 + 100; ++i) {
            cache_holder->insert("key_" + std::to_string(i), new std::string("val_" + std::to_string(i)));
        }
    }

    TEST(lru_cache, erase_basic) {
        srand(time(0));
        auto cache_holder = std::make_unique<Cache<std::string, std::string>>(5000);

        cache_holder->register_clean_handle([](const std::string &key, std::string *val) {
            delete val;
        });
        cache_holder->insert("key_0", new std::string("val_0"));
        cache_holder->erase("key_0");
        cache_holder->erase("key_0");
        cache_holder->erase("key_0");
    }

    TEST(lru_cache, erase) {
        srand(time(0));
        constexpr int N = 5000;
        auto cache_holder = std::make_unique<Cache<std::string, std::string>>(N);

        cache_holder->register_clean_handle([](const std::string &key, std::string *val) {
            delete val;
        });
        for (int i = 0; i < 1.5 * N; ++i) {
            cache_holder->insert("key_" + std::to_string(i), new std::string("val_" + std::to_string(i)));
            if (rand() & 1) {
                cache_holder->erase("key_" + std::to_string(i));
            }
        }
        for (int i = 0; i < 1.5 * N; ++i) {
            cache_holder->erase("key_" + std::to_string(i));
        }
    }

    TEST(lru_cache, get) {
        constexpr int N = 5000;
        auto cache_holder = std::make_unique<Cache<std::string, std::string>>(N);

        cache_holder->register_clean_handle([](const std::string &key, std::string *val) {
            delete val;
        });
        for (int i = 0; i < 1.5 * N; ++i) {
            cache_holder->insert("key_" + std::to_string(i), new std::string("val_" + std::to_string(i)));
        }
        for (int i = 0; i < 1.5 * N; ++i) {
            EXPECT_EQ(*(cache_holder->get("key_" + std::to_string(i))->val), "val_" + std::to_string(i));
            cache_holder->erase("key_" + std::to_string(i));
        }
    }

    TEST(lru_cache, all) {
        srand(time(0));
        constexpr int N = 5000;
        auto cache_holder = std::make_unique<Cache<std::string, std::string>>(N);

        cache_holder->register_clean_handle([](const std::string &key, std::string *val) {
            delete val;
        });
        for (int i = 0; i < 1.5 * N; ++i) {
            cache_holder->insert("key_" + std::to_string(i), new std::string("val_" + std::to_string(i)));
            if (i & 1) {
                cache_holder->erase("key_" + std::to_string(i));
            }
        }
        for (int i = 0; i < 1.5 * N; ++i) {
            if (i & 1) {
                EXPECT_EQ(cache_holder->get("key_" + std::to_string(i)), nullptr);
            } else {
                EXPECT_EQ(*(cache_holder->get("key_" + std::to_string(i))->val), "val_" + std::to_string(i));
                cache_holder->erase("key_" + std::to_string(i));
            }
        }
    }
}