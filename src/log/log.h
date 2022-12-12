//
// Created by qianyy on 2022/12/12.
//
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifndef SMALLKV_LOG_H
#define SMALLKV_LOG_H

namespace smallkv {
    std::shared_ptr<spdlog::logger> get_logger();
}

#endif //SMALLKV_LOG_H
