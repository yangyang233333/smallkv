//
// Created by qianyy on 2023/1/21.
//
#include <memory>
#include <string_view>
#include <utility>
#include "db/status.h"
#include "log_format.h"

#ifndef SMALLKV_WAL_WRITER_H
#define SMALLKV_WAL_WRITER_H
namespace smallkv {
    /*
     * leveldb中的设计：
     * WAL主要是用来防止宕机导致的数据丢失。
     * WAL中的内存由多个连续的BLOCK(默认为32KB)组成。数据排布如下所示：
     *          +-------+
     *          | BLOCK |
     *          +-------+
     *          |  ...  |
     *          +-------+
     *          | BLOCK |
     *          +-------+
     * 注：如果BLOCK不足32KB会被补全到32KB
     *
     * BLOCK由若干个Log Record组成，每个Log Record的schema如下所示：
     *         +--------------+---------+----------+------+
     *         | checksum(4B) | len(2B) | type(1B) | data |
     *         +--------------+---------+----------+------+
     * type的作用主要是防止某条数据过大(大于BLOCK)，需要跨BLOCK存储的情况。
     * type取值如下所示：
     * FullType: 表示当前Log Record存储了完整数据；
     * FirstType ~ LastType: 数据的第一部分~最后一部分
     *
     *
     * 注意：这里的WAL参考了leveldb的设计方法，即拥有两层write结构。
     * 首先多条数据数据(例如add <key_0, val_0>; add <key_1, val_1>; del <key_0>; ... )
     * 由上层的WriteBatch封装后称为一个data，然后调用底层的WALWriter一次性写入磁盘。
     *
     *
     *
     * 由于之前已经实现了FileWriter，所以这里不在按照上面的实现，可以简化一下：
     * 单条数据schema如下：
     *         +--------------+---------+------+
     *         | checksum(4B) | len(4B) | data |
     *         +--------------+---------+------+
     * 不断append即可。
     *
     *
     * */
    class FileWriter;

    class WALWriter final {
    public:
        // 写入的文件路径
        explicit WALWriter(std::shared_ptr<FileWriter> writableFile) : writableFile(std::move(writableFile)) {}

        ~WALWriter();

        // 添加一个log，默认情况下写完后进行flush
        DBStatus AddLog(const std::string_view &log, bool need_flush = true);

    private:
        std::shared_ptr<FileWriter> writableFile;
    };
}
#endif //SMALLKV_WAL_WRITER_H
