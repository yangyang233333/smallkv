//
// Created by qianyy on 2023/1/23.
//
#include <cstdlib>

#ifndef SMALLKV_DEFAULT_ALLOC_H
#define SMALLKV_DEFAULT_ALLOC_H

namespace smallkv {
    // 一般不用DefaultAlloc
    // 本类主要用来替换FreeListAllocate进行debug，稳定后还是使用FreeListAllocate。
    class DefaultAlloc {
    public:
        DefaultAlloc() = default;

        ~DefaultAlloc() = default;

        void *Allocate(int32_t n);

        void Deallocate(void *p, int32_t n);

        void *Reallocate(void *p, int32_t old_size, int32_t new_size);
    };
}

#endif //SMALLKV_DEFAULT_ALLOC_H
