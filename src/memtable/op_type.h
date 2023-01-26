//
// Created by qianyy on 2023/1/26.
//

#ifndef SMALLKV_OP_TYPE_H
#define SMALLKV_OP_TYPE_H
namespace smallkv {
    enum OpType {
        kDeletion = 0x1, // 删除
        kAdd = 0x2,      // 添加
        kUpdate = 0x3,   // 更新
    };
}
#endif //SMALLKV_OP_TYPE_H
