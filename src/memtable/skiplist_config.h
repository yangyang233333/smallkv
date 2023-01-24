//
// Created by qianyy on 2023/1/23.
//

#ifndef SMALLKV_SKIPLIST_CONFIG_H
#define SMALLKV_SKIPLIST_CONFIG_H
namespace smallkv {
    struct SkipListConfig {
        SkipListConfig() = delete;

        // 此处是按照leveldb的设置，最大为12层
        static constexpr int kMaxHeight = 12;
    };
}
#endif //SMALLKV_SKIPLIST_CONFIG_H
