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
#include <absl/crc/crc32c.h>

using namespace std;

int main() {
    auto logger = smallkv::log::get_logger();
    string buf = "56fdg4g6f5d4";
    auto crc_val = absl::ComputeCrc32c(buf);
    cout << crc_val << endl;

    return 0;
}

