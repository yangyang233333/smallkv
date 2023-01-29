//
// Created by qianyy on 2023/1/29.
//
#include <iostream>
#include <string_view>

#ifndef SMALLKV_PRINT_H
#define SMALLKV_PRINT_H
namespace smallkv::cli {
    // 启动时打印欢迎信息
    void PrintWelcome();

    // 类似$
    void PrintStart();

    // 打印结果
    void PrintResult(const std::string_view &result);
}
#endif //SMALLKV_PRINT_H
