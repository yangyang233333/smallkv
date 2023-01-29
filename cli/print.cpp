//
// Created by qianyy on 2023/1/29.
//

#include "print.h"

namespace smallkv::cli {
    void PrintWelcome() {
        std::cout << "=======================" << std::endl;
        std::cout << "Hello from smallkv-cli." << std::endl;
        std::cout << "=======================" << std::endl;
    }

    void PrintStart() {
        std::cout << "smallkv> ";
    }

    void PrintResult(const std::string_view &result) {
        std::cout << result << std::endl;
    }
}