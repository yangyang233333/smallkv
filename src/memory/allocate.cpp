//
// Created by qianyy on 2022/12/14.
//
#include <cassert>
#include "allocate.h"
#include "log/log.h"

namespace smallkv {

    auto logger = log::get_logger();

    FreeListAllocate::FreeListAllocate() {
        // 内存槽初始化
        memory_slot.fill(nullptr);

        // 初始化内存池
        mem_pool_start = (char *) malloc(CHUNK_SIZE);
        mem_pool_size = CHUNK_SIZE;
    }

    int FreeListAllocate::get_index(int32_t n) {
        return static_cast<int>(n + 7) / 8 - 1;
    }

    void *FreeListAllocate::Allocate(int32_t n) {
        if (n <= 0) {
            logger->error("The n is negative or zero. n = {}", n);
            return nullptr;
        }
        //如果分配的内存大于最大的对象大小（这里设置为4KB），直接调用系统内存分配器
        if (n > MAX_OBJ_SIZE) {
            return malloc(n);
        }

        auto slot_index = get_index(n);
        // 对应的内存槽上面没有空闲内存，需要先填充内存槽才可以分配
        if (memory_slot[slot_index] == nullptr) {
            fill_slot(slot_index); // 填充下标为slot_index的内存槽
        }

        BlockNode *ret = memory_slot[slot_index];
        memory_slot[slot_index] = ret->next;
        return ret;
    }

    void FreeListAllocate::fill_slot(int32_t slot_index) {
        // 填充逻辑：一次性申请FILL_BLOCK_CNT个block（默认为10个），挂载到memory_slot[slot_index]上面
        int32_t block_size = (slot_index + 1) * 8; // 当前内存槽的block大小
        int32_t needed_size = FILL_BLOCK_CNT * block_size;
        if (mem_pool_size >= needed_size) {// 内存池大小完全满足需要
            for (int i = 0; i < FILL_BLOCK_CNT; ++i) {
                auto node = reinterpret_cast<BlockNode *>(mem_pool_start + i * block_size);
                //bugfix: 需要让当前slot的最后一个block的next指针为空，否则会导致无法调用fill_mem_pool;
                if (i == 0) {
                    node->next = nullptr;
                }
                node->next = memory_slot[slot_index];
                memory_slot[slot_index] = node;
            }
            // 更新内存池的起始地址和大小
            mem_pool_start += needed_size;
            mem_pool_size -= needed_size;
        } else if (mem_pool_size >= block_size) { // 内存池大小至少能满足一个以上的block的需要
            int32_t cnt = mem_pool_size / block_size;
            for (int i = 0; i < cnt; ++i) {
                auto node = reinterpret_cast<BlockNode *>(mem_pool_start + i * block_size);
                if (i == 0) {
                    node->next = nullptr;
                }
                node->next = memory_slot[slot_index];
                memory_slot[slot_index] = node;
            }
            // 更新内存池的起始地址和大小
            mem_pool_start += cnt * block_size;
            mem_pool_size -= cnt * block_size;
        } else { // 内存池大小连一个block都无法满足
            // 内存按照8字节对其，所以内存块的大小一定是8的整数倍
            assert(mem_pool_size % 8 == 0);

            // 将内存池的剩余部分挂载到slot上面，避免浪费
            if (mem_pool_size >= 8) {
                int32_t target_slot_index = get_index(mem_pool_size);
                auto node = reinterpret_cast<BlockNode *>(mem_pool_start);
                node->next = memory_slot[target_slot_index];
                memory_slot[target_slot_index]->next = node;
                mem_pool_start = nullptr;
                mem_pool_size = 0;
            }

            // 重新申请一块内存池
            logger->debug("func fill_mem_pool is called.");
            fill_mem_pool();

            // 重新执行本函数
            fill_slot(slot_index);
        }
    }

    void FreeListAllocate::fill_mem_pool() {
        mem_pool_start = (char *) malloc(CHUNK_SIZE);
        if (mem_pool_start == nullptr) {
            logger->error("Memory allocation failed.");
            return;
        }
        logger->debug("{}MB memory is allocated once.", CHUNK_SIZE / 1024 / 1024);
        mem_pool_size = CHUNK_SIZE;
    }

    void FreeListAllocate::Deallocate(void *p, int32_t n) {
        if (p == nullptr || n == 0) {
            logger->error("p cannot be nullptr, n cannot be zero. p={}, n={}", p, n);
            return;
        }

        // 大于MAX_OBJ_SIZE是malloc分配的，也由free回收
        if (n > MAX_OBJ_SIZE) {
            free(p);
            p = nullptr;
            return;
        }
        auto slot_index = get_index(n);
        auto node = static_cast<BlockNode *>(p);
        node->next = memory_slot[slot_index];
        memory_slot[slot_index] = node;
    }

    void *FreeListAllocate::Reallocate(void *p, int32_t old_size, int32_t new_size) {
        Deallocate(p, old_size);
        return Allocate(new_size);
    }
}
