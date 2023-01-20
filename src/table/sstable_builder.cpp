//
// Created by qianyy on 2023/1/16.
//

#include <cassert>
#include "sst_config.h"
#include "utils/codec.h"
#include "footer_builder.h"
#include "sstable_builder.h"
#include "file/file_writer.h"
#include "data_block_builder.h"
#include "filter/bloom_filter.h"
#include "index_block_builder.h"
#include "filter_block_builder.h"

namespace smallkv {
    SSTableBuilder::SSTableBuilder(int32_t keys_num, std::shared_ptr<FileWriter> _fileWriter,
                                   double false_positive) : fileWriter(std::move(_fileWriter)) {
        _keys_num = keys_num;
        _false_positive = false_positive;
        dataBlockBuilder = std::make_shared<DataBlockBuilder>();
        indexBlockBuilder = std::make_shared<IndexBlockBuilder>();
        filterBlockBuilder = std::make_shared<FilterBlockBuilder>(keys_num, false_positive);
        footerBuilder = std::make_shared<FooterBuilder>();
        logger = log::get_logger();
    }

    DBStatus SSTableBuilder::add(const std::string &key, const std::string &val) {
        if (key.empty()) {
            return Status::Success;
        }

        // 生成filter
        filterBlockBuilder->add(key);
        //写入dataBlock
        dataBlockBuilder->add(key, val);
        ++key_count;
        pre_key = key;
        // 如果DataBlockBuilder大小超过限制，则应该把DataBlockBuilder落盘，然后清空DataBlockBuilder
        if (dataBlockBuilder->size() > SSTConfigInfo::MAX_DATA_BLOCK_SIZE) {
            // 当add_restart_points函数被调用完成的时候，表明当前DataBlock
            // 已经完全写完，所有必备数据都写到了dataBlockBuilder的_data中。
            // 此时持久化只需要持久化_data即可
            dataBlockBuilder->finish_data_block();
            // 持久化dataBlockBuilder中的数据
            fileWriter->append(dataBlockBuilder->data().data(),
                               static_cast<int32_t>(dataBlockBuilder->data().size()));

            // 更新FilterBlock的Offset
            FilterBlock_offset.offset += static_cast<int32_t>(dataBlockBuilder->data().size());

            // 给已经持久化的DataBlock生成IndexBlock信息
            // 找到最短的前缀 shortest_key : pre_key < shortest_key <= key
            auto shortest_key = find_shortest_key(pre_key, key);
            OffsetInfo data_block_offset_info{};
            data_block_offset_info.size = static_cast<int32_t>(dataBlockBuilder->data().size());
            data_block_offset_info.offset = FilterBlock_offset.offset;
            indexBlockBuilder->add(shortest_key, data_block_offset_info);
            index_data.append(indexBlockBuilder->data()); // 暂存起来
            indexBlockBuilder->clear(); // 清空indexBlockBuilder的旧信息

            // 持久化完成后，清空当前dataBlockBuilder
            dataBlockBuilder->clear();
        }
        return Status::Success;
    }

    std::string SSTableBuilder::find_shortest_key(
            const std::string &key1, const std::string &key2) {
        assert(!key1.empty());
        assert(!key2.empty());
        assert(key1 < key2);
        int cnt = 0;
        for (int i = 0; i < std::min(key1.size(), key2.size()); ++i) {
            if (key1[i] == key2[i]) {
                ++cnt;
            }
        }
        std::string shortest_key;
        if (key1.size() == cnt) {
            shortest_key = key1 + " ";
            return shortest_key;
        }
        shortest_key = key1.substr(0, cnt + 1);
        shortest_key.back() += 1;
        return shortest_key;
    }

    DBStatus SSTableBuilder::finish_sst() {
        // 1. 保存最后一个DataBlock
        if (dataBlockBuilder->size() > 0) {
            dataBlockBuilder->finish_data_block();
            fileWriter->append(dataBlockBuilder->data().data(),
                               static_cast<int32_t>(dataBlockBuilder->data().size()));

            // 保存最后一个DataBlock的IndexBlock
            OffsetInfo offset_info{};
            offset_info.size = static_cast<int32_t>(dataBlockBuilder->data().size());
            offset_info.offset = FilterBlock_offset.offset;
            pre_key.back() += 1;
            indexBlockBuilder->add(pre_key, offset_info);
            index_data.append(indexBlockBuilder->data()); // 暂存起来
            indexBlockBuilder->clear(); // 清空indexBlockBuilder的旧信息

            // 更新FilterBlock的Offset
            FilterBlock_offset.offset += static_cast<int32_t>(dataBlockBuilder->data().size());

            dataBlockBuilder->clear(); // 持久化完成后，清空当前dataBlockBuilder
        }
        filterBlockBuilder->finish_filter_block();

        // 保存FilterBlock的size
        FilterBlock_offset.size = static_cast<int32_t>(filterBlockBuilder->data().size());

        // 2. 保存filter
        filterBlockBuilder->finish_filter_block(); // 真正的构建filter
        fileWriter->append(filterBlockBuilder->data().data(),
                           static_cast<int32_t>(filterBlockBuilder->data().size()));

        // 3. 持久化index_data
        fileWriter->append(index_data.c_str(), static_cast<int32_t>(index_data.size()));

        // 4. 持久化Footer
        assert(static_cast<int32_t>(index_data.size()) > 0);
        IndexBlock_offset.offset = FilterBlock_offset.offset + FilterBlock_offset.size;
        IndexBlock_offset.size = static_cast<int32_t>(index_data.size());
        footerBuilder->add(FilterBlock_offset, IndexBlock_offset);
        fileWriter->append(footerBuilder->data().data(),
                           static_cast<int32_t>(footerBuilder->data().size()));

        // 持久化
        fileWriter->flush();
        fileWriter->close();

        // 清空字段
        dataBlockBuilder->clear();
        indexBlockBuilder->clear();
        filterBlockBuilder = std::make_shared<FilterBlockBuilder>(
                _keys_num, _false_positive);
        // todo : fileWriter字段如何清空？？这里存在设计缺陷！！
        // 关于上面todo的解释：每个.sst文件只会放一个SSTable，所以在SSTableBuilder外部建一个FileWriter指针，
        // 然后传给SSTableBuilder来创建这个文件，创建完成后，这个FileWriter指针应该会自动销毁
        footerBuilder->clear();

        FilterBlock_offset.clear();
        IndexBlock_offset.clear();
        index_data.clear();
        pre_key.clear();
        pre_offset_info.clear();

        return Status::Success;
    }
}