//
// Created by qianyy on 2022/12/14.
//
#include <cstdint>
#include <vector>
#include <gtest/gtest.h>

#ifndef SMALLKV_ALLOCATE_H
#define SMALLKV_ALLOCATE_H

namespace smallkv {
    /*
     * 设置512个slot(内存槽)，slot_0对应8字节，slot_1对应16字节，slot_511对应4096字节
     * 记这里的8B, 16B, 4096B为block。
     * slot中挂载的都是空闲的block，当有内存请求时：
     * 0. 如果请求的内存大于4KB，那么直接调用系统的malloc（本分配器主要优化小内存对象的分配，
     *    不需要管大对象）；
     * 1. 首先定位到具体的内存槽，如果该内存槽不空，则跳转第2步； 如果该内存槽为空，则跳转第3步；
     * 2. 此时内存槽上面有空闲的block，分配即可；
     * 3. 此时内存槽上面没有空闲的block，那么在内存池中取出1个block返回，如果内存池空间也不够，
     *    那么向系统申请20个block空间，1个返回，9个挂载到内存槽上面， 10个放到空闲内存池；
     * */
    union BlockNode {
        //若被使用，表示当前block未被使用，next表示下一个空闲block的地址
        BlockNode *next;

        //若被使用，表示当前block已经被使用，block_addr表示当前block所存的数据的地址
        void *block_addr;
    };
//    0 0 0 0 0 0 0 0

    // 为KV引擎中小对象内存分配进行特别优化的Allocator
    class FreeListAllocate final {
    public:
        FreeListAllocate();

        ~FreeListAllocate() = default;

        // 分配n字节的内存
        void *Allocate(int32_t n);

        // 释放p所指的n字节内存
        void Deallocate(void *p, int32_t n);

        // 将p所指的内存由old_size字节扩容到new_size字节
        void *Reallocate(void *p, int32_t old_size, int32_t new_size);

    private:
        // 填充内存槽（初始时内存槽上面没有挂载block或者被消耗完毕的时候，调用此函数进行填充）
        // 本函数逻辑是：
        // 1. 如果空闲内存池大小足够，就从空闲内存池中取出`n * bytes`组成block挂载到slot上面；
        // 2. 如果空闲内存池大小不足，就调用fill_mem_pool对内存池进行填充，然后再按照步骤1；
        void fill_slot(int32_t slot_index);

        // 如果内存池大小不足，就向系统申请一块内存作为内存池
        // 本函数逻辑是: 将原来的内存池的剩余部分挂载到slot上面（避免浪费），然后重新分配
        void fill_mem_pool();

        // 根据请求的内存大小，计算出最佳的slot。
        // 例如请求7B，那么应该返回8B对应的slot索引，也就是0
        static inline int32_t get_index(int32_t n);

    private:
        // 内存槽，memory_slot[0]指向空闲的8B区块链表，memory_slot[1]指向空闲的16B区块链表... ...
        std::vector<BlockNode *> memory_slot;

        // 空闲内存池的开始
        char *mem_pool_start = nullptr;
        // 空闲内存池的长度
        int32_t mem_pool_size = 0;

        std::mutex _mutex; // 待定，暂时无用
    private:
        constexpr static int32_t SLOT_NUM = 512; // 512个内存槽
        constexpr static int32_t ALIGN_BYTES = 8; // 按照8字节对齐
        constexpr static int32_t MAX_OBJ_SIZE = 4 * 1024; // 设置最大的小对象为4K，超过4K就直接调用系统的malloc
        constexpr static int32_t CHUNK_SIZE = 4 * 1024 * 1024; // 每次扩展4MB的空闲内存池
        constexpr static int32_t FILL_BLOCK_CNT = 10; // 每次向内存槽填充的block数量，默认为10个
    };
}
#endif //SMALLKV_ALLOCATE_H
