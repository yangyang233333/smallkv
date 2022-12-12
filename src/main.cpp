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
using namespace std;

int main() {
    spdlog::info("Hello");
    auto h = smallkv::murmur_hash2("123", 3);
    spdlog::info(h);


    vector<uint8_t> a;
    cout << a.size() << endl;
    for (int i = 0; i < 2000 * 10000; ++i) {
        a.resize(i, 1);
    }
    cout << a.size() << endl;

    auto logger = spdlog::stdout_color_mt("console");
    logger->info("hello");
    return 0;
}

