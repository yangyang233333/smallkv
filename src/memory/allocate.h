//
// Created by qianyy on 2022/12/14.
//

#ifndef SMALLKV_ALLOCATE_H
#define SMALLKV_ALLOCATE_H
namespace smallkv {
    union FreeListNode {
        FreeListNode *next;
        char *data;
    };

    class FreeListAllocate final {
    public:
        FreeListAllocate() = default;

    private:
        constexpr static int align_bytes = 8; // 8字节对齐
        constexpr static int max_obj_bytes = 4096; // 设置最大的小对象为4K，超过4K就直接调用系统的malloc

    };
}
#endif //SMALLKV_ALLOCATE_H
