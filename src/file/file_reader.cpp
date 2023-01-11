//
// Created by qianyy on 2023/1/11.
//
#include <string>
#include <unistd.h>
#include <cassert>
#include <fcntl.h>
#include "../log/log.h"
#include "file_reader.h"

namespace smallkv {
    FileReader::FileReader(const std::string &file_path) {
        if (access(file_path.c_str(), F_OK) != F_OK) {
            log::get_logger()->error("{} cannot access.", file_path);
        } else {
            fd = open(file_path.c_str(), O_RDONLY);
            if (fd < 0) {
                log::get_logger()->error("{} cannot open.", file_path);
            }
        }
    }

    FileReader::~FileReader() {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }

    DBStatus FileReader::read(void *buf, int32_t count, int32_t offset) const {
        if (buf == nullptr) {
            return Status::InvalidArgs;
        }
        if (fd == -1) {
            log::get_logger()->error("fd == -1");
            return Status::ExecFailed;
        }
        auto cnt = pread(fd, buf, count, offset);
        if (cnt != count) {
            log::get_logger()->error("pread exec failed.");
            return Status::ExecFailed;
        }
        return Status::Success;
    }
}