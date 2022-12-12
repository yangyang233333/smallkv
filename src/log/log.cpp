//
// Created by qianyy on 2022/12/12.
//
#include <mutex>
#include "log.h"

namespace smallkv {
    std::shared_ptr<spdlog::logger> logger = nullptr;
    std::mutex _mutex;

    // todo:此处加锁开销略大，后续优化可以使用双检查锁+内存屏障的方法来减小开销
    std::shared_ptr<spdlog::logger> get_logger() {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (logger == nullptr) {
            logger = spdlog::stdout_color_mt("console");
        }
        return logger;
    }
}
