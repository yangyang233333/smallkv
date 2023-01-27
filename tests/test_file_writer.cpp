//
// Created by qianyy on 2023/1/11.
//

#include <memory>
#include <gtest/gtest.h>
#include <string>
#include "../src/file/file_writer.h"
#include "../src/log/log.h"

namespace smallkv::unittest {
    TEST(file_writer, basic) {
        auto logger = log::get_logger();
        std::string path = "./build/tests.sst";
        auto fwriter = std::make_unique<FileWriter>(path, true);

        std::string data1 = "123gfds4h6.1 s0 3ds 4g00 x_";
        std::string data2 = "  7112gdf455 4 56 u 455u 123fgh12fgn  ";
        std::string data3 = ".*-123 34fe r65 4r1 0m1 j0.10,/,.";
        fwriter->append(data1.data(), static_cast<int32_t>(data1.size()));
        fwriter->append(data2.data(), static_cast<int32_t>(data2.size()));
        fwriter->append(data3.data(), static_cast<int32_t>(data3.size()));
        fwriter->sync();
        fwriter->close();
        auto _ = std::system(("rm -rf " + path).c_str());
    }
}