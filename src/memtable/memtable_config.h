//
// Created by qianyy on 2023/1/25.
//

#ifndef SMALLKV_MEMTABLE_CONFIG_H
#define SMALLKV_MEMTABLE_CONFIG_H
namespace smallkv {
    struct MemTableConfig {
        MemTableConfig() = delete;

        static constexpr int MAX_KEY_NUM = 4096; // 超过4096个Key，就新建一个memtable
    };
}
#endif //SMALLKV_MEMTABLE_CONFIG_H
