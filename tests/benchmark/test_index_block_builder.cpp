//
// Created by qianyy on 2023/1/16.
//
#include <memory>
#include <gtest/gtest.h>
#include <string>
#include <cstring>

namespace smallkv {
    /*
     * -O0测试结果：N=100000000
     * benchmark._memcpy (52 ms)
     * benchmark.bit_compute (112 ms)
     *
     * */
    namespace benchmark_config {
        uint32_t data = 0x12345678;
        int N = 100000000;
    }

    TEST(benchmark, bit_compute) {
        char buf[4];
        for (int i = 0; i < benchmark_config::N; ++i) {
            buf[0] = benchmark_config::data & 0xff;
            buf[1] = (benchmark_config::data >> 8) & 0xff;
            buf[2] = (benchmark_config::data >> 16) & 0xff;
            buf[3] = (benchmark_config::data >> 24) & 0xff;
        }
    }

    TEST(benchmark, _memcpy) {
        char buf[4];
        for (int i = 0; i < benchmark_config::N; ++i) {
            memcpy(buf, &benchmark_config::data, 4);
        }
    }
}