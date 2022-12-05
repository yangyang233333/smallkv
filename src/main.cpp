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

#include "utils/murmur_hash2.h"
#include "spdlog/spdlog.h"

using namespace std;

int main() {
    spdlog::info("Hello");
    auto h = smallkv::murmur_hash2("123", 3);
    spdlog::info(h);
    return 0;
}


















