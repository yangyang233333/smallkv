//
// Created by qianyy on 2023/1/30.
//
#include <optional>
#include <cassert>
#include "sst_parser.h"
#include "filter/bloom_filter.h"
#include "filter/filter_policy.h"
#include "utils/codec.h"
#include "restart_point.h"

namespace smallkv {
    SSTParser::SSTParser(const std::string &sst_filepath) {
        fs::path path = sst_filepath;
        assert(fs::exists(path));
        is = path;
        logger = log::get_logger();
    }

    void SSTParser::ReadAll() {
        std::string temp;
        while (getline(is, temp)) {
            sst_content.append(temp);
            temp.clear();
        }
    }

    void SSTParser::Parse() {
        ReadAll(); // sst文件一次性读到内存中，也就是 sst_content 中

        auto sst_ptr = sst_content.data();
        const auto SST_FILE_SIZE = sst_content.size();

        // 1. 解析Header
        {
            header.MetaBlock_OffsetInfo = *reinterpret_cast<OffsetInfo *>(sst_ptr + SST_FILE_SIZE - 16);
            header.IndexBlock_OffsetInfo = *reinterpret_cast<OffsetInfo *>(sst_ptr + SST_FILE_SIZE - 8);
        }
        logger->info("MetaBlock_OffsetInfo: size={}, offset={}",
                     header.MetaBlock_OffsetInfo.size, header.MetaBlock_OffsetInfo.offset);
        logger->info("IndexBlock_OffsetInfo: size={}, offset={}",
                     header.IndexBlock_OffsetInfo.size, header.IndexBlock_OffsetInfo.offset);

        /*
         * 草稿纸：
         * IndexBlock的 size = IndexBlock_OffsetInfo.size
         * IndexBlock的 offset = IndexBlock_OffsetInfo.offset
         * 所以：IndexBlock: sst_ptr[offset : offset + size]
         *
         * Filter的 size = MetaBlock_OffsetInfo.size
         * Filter的 offset = MetaBlock_OffsetInfo.offset
         * 所以: Filter = sst_ptr[offset : offset + size]
         *
         * DataBlock的 size = MetaBlock_OffsetInfo.offset
         * DataBlock的 offset = 0
         * 所以：DataBlock的 = sst_ptr[0 : size]
         *
         * */

        // 2. 解析IndexBlock
        {
            const int START = header.IndexBlock_OffsetInfo.offset;
            const int SIZE = header.IndexBlock_OffsetInfo.size;
            int start = START;
            while (start < START + SIZE) {
                // 每次遍历：递增 4 + key_size + 8
                int _shortest_key_size = *reinterpret_cast<int32_t *>(sst_ptr + start);
                IndexBlock block;
                // 注意：_shortest_key严格大于该data block的所有key
                block._shortest_key = std::move(std::string(sst_ptr + start + 4, _shortest_key_size));
                block.offsetInfo = *reinterpret_cast<OffsetInfo *>(sst_ptr + start + 4 + _shortest_key_size);
                start += 4 + _shortest_key_size + 8;
                index_blocks.push_back(std::move(block));
            }
            assert(start == START + SIZE);
        }

        // 3. 解析filter
        {
            const int START = header.MetaBlock_OffsetInfo.offset;
            const int SIZE = header.MetaBlock_OffsetInfo.size;
            int32_t hash_func_num = *reinterpret_cast<int32_t *>(START + SIZE - 4);
            std::string bits_array_ = std::move(std::string(sst_ptr + START, SIZE - 4));
            filterPolicy = std::make_shared<BloomFilter>();
            filterPolicy->create_filter2(hash_func_num, bits_array_); // 创建filter
        }
    }

    bool SSTParser::Exists(const std::string_view &key) {
        return filterPolicy->exists(key);
    }

    std::optional<std::string> SSTParser::Seek(const std::string_view &key) {
        if (!Exists(key)) {
            return std::nullopt;
        }

        int DataBlockIndex = FindKeyDBIndex(key);  // 找到key所在的DataBlock序号
        if (DataBlockIndex == -1) {
            logger->info("A bloomfilter misjudgment occurred. [DataBlockIndex == -1]");
            return std::nullopt;
        }

        char *SST_PTR = sst_content.data(); // SST_PTR 指的是整个 SST 的起始地址

        auto index_block = index_blocks[DataBlockIndex];
        auto ret = FindKeyInDB(key, SST_PTR + index_block.offsetInfo.offset,
                               SST_PTR + index_block.offsetInfo.offset + index_block.offsetInfo.size);
        return ret;
    }

    int SSTParser::FindKeyDBIndex(const std::string_view &key) {
        int index = -1;
        for (int i = 0; i < index_blocks.size(); ++i) {
            if (key < index_blocks[i]._shortest_key) {
                index = i;
                break;
            }
        }
        return index;
    }

    std::optional<std::string> SSTParser::FindKeyInDB(const std::string_view &key,
                                                      char *START, char *END) {
        const int Restart_Num = static_cast<int>(utils::DecodeFixed32(END - 12));
        auto a_ = utils::DecodeFixed64(END - 8);
        OffsetInfo Restart_Offset = *reinterpret_cast<OffsetInfo *>(&a_);

        // 解析单个rp (rp = restart point)
        // rp_start_ptr指该rp的起始地址，注意，RP长度固定为12B
        auto parse_rp = [](char *rp_start_ptr) {
            int record_num = static_cast<int>(utils::DecodeFixed32(rp_start_ptr));
            auto _a = utils::DecodeFixed64(rp_start_ptr + 4);
            OffsetInfo rp_offset = *reinterpret_cast<OffsetInfo *>(&_a);
            return std::make_tuple(record_num, rp_offset);
        };

        // 遍历重启点信息
        std::vector<RestartPoint> rps;
        for (int i = 0; i < Restart_Num; ++i) {
            const auto &[record_num, rp_offset] = parse_rp(START + Restart_Offset.offset + i * 12);
            auto fullkey = FindFullKeyByRP(START, record_num, rp_offset);
            if (key < fullkey) {
                break;
            }
            RestartPoint rp{record_num, rp_offset, fullkey};
            rps.push_back(std::move(rp));
        }
        if (rps.empty()) {
            logger->info("A misjudgment occurred by filter.");
            return std::nullopt; // 没找到，也就是假阳性
        }

        // rps最后一个RestartPoint就是key所在的位置
        const auto &RP = rps.back();
        const std::string_view FULLKEY = RP.fullkey;
        assert(key >= FULLKEY);
        // 顺序遍历该 RG
        std::string value;
        char *RG_START = START + RP.rp_offset.offset;

        // 解析record schema, ptr是该record的起始地址
        auto parse_record = [](char *ptr) {
            int shared_key_len = static_cast<int>(utils::DecodeFixed32(ptr));
            int unshared_key_len = static_cast<int>(utils::DecodeFixed32(ptr));
            int value_len = static_cast<int>(utils::DecodeFixed32(ptr));
            int record_size = 12 + unshared_key_len + value_len;
            std::string unshared_key_content, value_content;
            unshared_key_content = std::string(ptr + 12, unshared_key_len);
            value_content = std::string(ptr + 12 + unshared_key_len, value_len);
            return std::make_tuple(record_size, shared_key_len,
                                   unshared_key_content, value_content);
        };

        for (int i = 0; i < RP.record_num; ++i) {
            const auto &[record_size, shared_key_len, unshared_key_content, value_content]
                    = parse_record(RG_START);
            RG_START += record_size;
            // 差值压缩的逆过程
            // todo: 此处的.data()拼接可能不够高效，需要优化
            auto repaired_key = FULLKEY.substr(0, shared_key_len).data() + unshared_key_content;
            if (repaired_key == key) {
                value = value_content; //找到了
                break;
            }
        }
        assert(RG_START <= START + RP.rp_offset.offset + RP.rp_offset.size);

        if (value.empty()) {
            return std::nullopt;
        }
        return value;
    }

    std::string SSTParser::FindFullKeyByRP(char *START, int record_num,
                                           const OffsetInfo &rg_offset) {
        assert(record_num > 0);
        auto first_record_ptr = START + rg_offset.offset;
        int shared_key_len = static_cast<int>(utils::DecodeFixed32(first_record_ptr));
        assert(shared_key_len == 0); // 第一条key共享长度一定为0
        int unshared_key_len = static_cast<int>(utils::DecodeFixed32(first_record_ptr + 4));
        // int value_len = static_cast<int>(utils::DecodeFixed32(first_record_ptr + 8));
        assert(unshared_key_len > 0);
        std::string fullkey = std::string(first_record_ptr + 12, unshared_key_len);
        return fullkey;
    }
}
