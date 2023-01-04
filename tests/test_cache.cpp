//
// Created by qianyy on 2023/1/4.
//
#include <memory>
#include <string>
#include <gtest/gtest.h>
#include "../src/cache/lru.h"
#include "../src/cache/node.h"
#include "../src/cache/cache.h"
#include "../src/cache/cache_policy.h"

namespace smallkv {
    TEST(lru_cache, insert1) {
        auto cache_holder = std::make_unique<Cache<std::string, std::string>>(5000);

        cache_holder->register_clean_handle([](const std::string &key, std::string *val) {
            delete val;
        });
        cache_holder->insert("key1", new std::string("val1"));
        cache_holder->insert("key2", new std::string("val2"));
        cache_holder->insert("key3", new std::string("val3"));
    }

    TEST(lru_cache, insert2) {
        auto cache_holder = std::make_unique<Cache<std::string, std::string>>(2);

        cache_holder->register_clean_handle([](const std::string &key, std::string *val) {
            delete val;
        });
        cache_holder->insert("key1", new std::string("val1"));
        cache_holder->insert("key2", new std::string("val2"));
        cache_holder->insert("key3", new std::string("val3"));
    }

    TEST(lru_cache, erase) {

    }

    TEST(lru_cache, get) {

    }

    TEST(lru_cache, all) {

    }
}