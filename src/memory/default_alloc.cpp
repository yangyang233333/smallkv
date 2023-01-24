//
// Created by qianyy on 2023/1/23.
//
#include "default_alloc.h"

namespace smallkv {
    void *DefaultAlloc::Allocate(int32_t n) {
        return malloc(n);
    }

    void DefaultAlloc::Deallocate(void *p, int32_t n) {
        free(p);
    }

    void *DefaultAlloc::Reallocate(void *p, int32_t old_size, int32_t new_size) {
        return realloc(p, new_size);
    }
}