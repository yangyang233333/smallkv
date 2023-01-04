//
// Created by qianyy on 11/7/22.
//

#include <stack>
#include <queue>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <memory>
#include <cassert>
#include <cmath>
#include "utils/murmur_hash2.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "log/log.h"
#include "filter/filter_policy.h"
#include "filter/bloom_filter.h"
#include <nlohmann/json.hpp>
#include <fstream>

using namespace std;

mutex m;

void fn(bool need_lock) {
    // 如何消除if分支？
    if (need_lock) {
        lock_guard<mutex> lockGuard(m);
        // xxx
    } else {
        //xxx
    }
}

template<typename LockType>
void fn_enhanced() {
    LockType lockType;
    lock_guard<LockType> lockGuard(lockType);
//    xxx
}



int main() {
    auto logger = smallkv::log::get_logger();
    logger->error("hello, {}", "wxq");
    logger->info("hello, {}", "qianyy");


    return 0;
}

