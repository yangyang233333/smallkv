//
// Created by qianyy on 2023/1/30.
//
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include "header.h"
#include "index_block.h"
#include "log/log.h"

#ifndef SMALLKV_SST_PARSER_H
#define SMALLKV_SST_PARSER_H
namespace smallkv {
    /*
     * SST文件解析器
     *
     * SST结构
     * SSTable的数据排布schema：
     *           +----------------+
     *           |   DataBlock_1  |
     *           +----------------+
     *           |       ...      |
     *           +----------------+
     *           |   DataBlock_N  |
     *           +----------------+
     *           |    MetaBlock   |
     *           +----------------+
     *           |  IndexBlock_1  |
     *           +----------------+
     *           |       ...      |
     *           +----------------+
     *           |  IndexBlock_N  |
     *           +----------------+
     *           |     Footer     |
     *           +----------------+
     *
     * DataBlock数据排布如下:
     *               +---------------+
     *               |   Record_1    |
     *               +---------------+
     *               |     ...       |
     *               +---------------+
     *               |   Record_N    |
     *               +---------------+
     *               |   Restart_1   |
     *               +---------------+
     *               |     ...       |
     *               +---------------+
     *               |   Restart_K   |
     *               +---------------+
     *               |  Restart_Num  |
     *               +---------------+
     *               | Restart_Offset|
     *               +---------------+
     *
     *     Record的schema：
     *           +--------------------+----------------------+---------------+----------------------+---------------+
     *           | shared_key_len(4B) | unshared_key_len(4B) | value_len(4B) | unshared_key_content | value_content |
     *           +--------------------+----------------------+---------------+----------------------+---------------+
     *
     *     Restart_Point的schema如下：
     *            +----------------+----------------+
     *            | record_num(4B) | OffsetInfo(8B) |
     *            +----------------+----------------+
     *
     *     IndexBlock schema
     *         +------------------------+---------------+-----------------+
     *         | _shortest_key_size(4B) | _shortest_key | _offsetInfo(8B) |
     *         +------------------------+---------------+-----------------+
     *
     *     Footer模块数据schema如下所示：
     *         +---------------------------+----------------------------+
     *         | MetaBlock_OffsetInfo (8B) | IndexBlock_OffsetInfo (8B) |
     *         +---------------------------+----------------------------+
     *
     * */
    namespace fs = std::filesystem;

    class FilterPolicy;

    /*
     * 类使用流程：
     * SSTParser() ==> Parse() ==> Seek(key)
     *
     * */
    class SSTParser {
    public:
        // 传入sst文件路径
        explicit SSTParser(const std::string &sst_filepath);

        // 解析sst文件
        void Parse();

        // 如果key存在，找到对应的key-value, 返回value; 不存在则返回nullopt
        std::optional<std::string> Seek(const std::string_view &key);

    private:
        // 找到 key 所在的data block的序号，从0开始；没找到则返回-1
        int FindKeyDBIndex(const std::string_view &key);

        // 在 DataBlock 二分查找key，找到则返回value，没找到返回nullopt
        // 注：DataBlock指[START, END]中间的字节，也就是在[START, END]二分查找key
        std::optional<std::string> FindKeyInDB(const std::string_view &key,
                                               char *START, char *END);

        // 根据重启点(RP=Restart Point)找到对应的Record Group的第一个full key
        // args:
        //   record_num: 对应的RG中有多少record; RG指Record Group
        //   rg_offset: 对应的RG的offset信息;
        std::string FindFullKeyByRP(char *START, int record_num,
                                                   const OffsetInfo &rg_offset);

        // 读取全部内容到 sst_content 中
        void ReadAll();

        // 如果key存在于此sst中，则返回true，否则返回false
        // 注意：这是基于布隆过滤器的判断，所以返回true可能存在假阳性
        bool Exists(const std::string_view &key);

    private:
        // filter
        std::shared_ptr<FilterPolicy> filterPolicy;

        // index block
        std::vector<IndexBlock> index_blocks;

        std::ifstream is;

        // todo：一次性读取到 sst_content 中，后续优化为分段读取，分段大小为 FILE_SEGMENT = 4M
        std::string sst_content;

        // todo： 【暂时无效，待实现】读取以 4M 为一个单位
        uint32_t FILE_SEGMENT = 4 * 1024 * 1024;

        // Offset
        Header header;

        std::shared_ptr<spdlog::logger> logger; // 日志
    };
}
#endif //SMALLKV_SST_PARSER_H
