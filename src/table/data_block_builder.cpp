//
// Created by qianyy on 2023/1/13.
//

#include <cassert>
#include "sst_config.h"
#include "data_block_builder.h"
#include "../utils/codec.h"
#include "../log/log.h"

namespace smallkv {

    // todo: 热点函数，后续需要更多优化
    DBStatus DataBlockBuilder::add(const std::string &key, const std::string &value) {
        if (key.empty()) {
            return Status::Success;
        }
        bool need_fullkey = false;
        if (record_num % SSTConfigInfo::RESTART_INTERVAL == 0) { // 判断是否需要插入重启点
            need_fullkey = true; // 第一条数据需要完整key
            record_group_offset.push_back(static_cast<int32_t>(get_records_size()));
        }
        ++record_num; // DataBlock中的Record的数量加一

        if (need_fullkey) {
            // 是Record Group的第一条数据，不需要差值压缩
            utils::PutFixed32(_data, 0);                                  // shared_key_len
            utils::PutFixed32(_data, static_cast<int32_t>(key.size()));   // unshared_key_len
            utils::PutFixed32(_data, static_cast<int32_t>(value.size())); // value_len
            _data.append(key);                                               // unshared_key_content
            _data.append(value);                                             // value_content
        } else {
            // 非第一条数据，进行差值压缩
            // 计算当前key和前一个key的公共部分
            auto min_len = std::min(key.size(), pre_key.size());
            int shared_key_len = 0;
            for (int i = 0; i < min_len; ++i) {
                if (key[i] == pre_key[i]) {
                    ++shared_key_len;
                } else {
                    break;
                }
            }
            int unshared_key_len = static_cast<int>(key.size()) - shared_key_len;

            // todo: 这里可以尝试一下variant编码
            utils::PutFixed32(_data, shared_key_len);
            utils::PutFixed32(_data, unshared_key_len);
            utils::PutFixed32(_data, static_cast<int32_t>(value.size()));
            _data.append(key.substr(shared_key_len));
            _data.append(value);
        }
        pre_key = key; // 更新前一个key

        return Status::Success;
    }

    DBStatus DataBlockBuilder::finish_data_block() {
        int restart_point_num = static_cast<int>((record_num - 0.5) / 16) + 1;
        int last_offset = static_cast<int>(_data.size()); // 记录_data的最后一个字节的位置
        for (int i = 0; i < record_group_offset.size(); ++i) {
            //
            //     Restart_Point的schema如下：
            //     +----------------+----------------+
            //     | record_num(4B) | OffsetInfo(8B) |
            //     +----------------+----------------+
            //
            int32_t _record_num; // 当前Record Group中的Record数量
            OffsetInfo _offsetInfo{0, record_group_offset[i]};
            if (i != record_group_offset.size() - 1) {
                _offsetInfo.size = record_group_offset[i + 1] - record_group_offset[i];
                _record_num = SSTConfigInfo::RESTART_INTERVAL;
            } else {
                _offsetInfo.size = last_offset - record_group_offset[i];
                _record_num = record_num % SSTConfigInfo::RESTART_INTERVAL;
            }
            utils::PutFixed32(_data, _record_num);
            utils::PutFixed64(_data, *reinterpret_cast<int64_t *>(&_offsetInfo));
        }
        // 重启点数量
        utils::PutFixed32(_data, restart_point_num);
        return Status::Success;
    }

    void DataBlockBuilder::clear() {
        record_group_offset.clear();
        pre_key.clear();
        _data.clear();
        record_num = 0;
    }
}