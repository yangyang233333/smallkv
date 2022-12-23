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
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace std;

void test_rapidjson() {
    // 1. Parse a JSON string into DOM.
    const char* json = R"({"project":"rapidjson","stars":10})";
    rapidjson::Document d;
    d.Parse(json);

    // 2. 修改DOM
    rapidjson::Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);

    // 3. 字符串化DOM
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);

    // Output {"project":"rapidjson","stars":11}
    std::cout << buffer.GetString() << std::endl;
}

int main() {

    auto logger = smallkv::log::get_logger();
    logger->error("hello, {}", "wxq");
    logger->info("hello, {}", "qianyy");

    return 0;
}

