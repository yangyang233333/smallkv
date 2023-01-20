//
// Created by qianyy on 2023/1/11.
//
#include "db/status.h"

#ifndef SMALLKV_FILE_READER_H
#define SMALLKV_FILE_READER_H

namespace smallkv {
    class FileReader final {
    public:
        explicit FileReader(const std::string &file_path);

        ~FileReader();

        // 类似unistd中的pread
        // 从offset处开始，读取count长度的内容到buf中
        DBStatus read(void *buf, int32_t count, int32_t offset = 0) const;

    private:
        int fd{0};
    };


}
#endif //SMALLKV_FILE_READER_H
