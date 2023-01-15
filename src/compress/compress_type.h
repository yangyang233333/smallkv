//
// Created by qianyy on 2023/1/12.
//

#ifndef SMALLKV_COMPRESS_TYPE_H
#define SMALLKV_COMPRESS_TYPE_H
namespace smallkv {
    // 压缩类型
    enum CompressType {
        NonCompress = 1, // 无压缩
        SnappyCompress = 2, // 使用snappy算法压缩 // todo: 实现snappy算法
    };

}
#endif //SMALLKV_COMPRESS_TYPE_H
