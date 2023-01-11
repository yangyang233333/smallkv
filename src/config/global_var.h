//
// Created by qianyy on 2023/1/11.
//
#include <string>

#ifndef SMALLKV_GLOBAL_VAR_H
#define SMALLKV_GLOBAL_VAR_H
namespace smallkv {
    // 所有的wal文件都保存在WAL_FILE_DIR下面
    extern std::string WAL_FILE_DIR;

    // 所有的sst文件都保存在SST_FILE_DIR下面
    extern std::string SST_FILE_DIR;

}
#endif //SMALLKV_GLOBAL_VAR_H
