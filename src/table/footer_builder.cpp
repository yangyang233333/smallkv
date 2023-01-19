//
// Created by qianyy on 2023/1/19.
//

#include "footer_builder.h"
#include "utils/codec.h"

namespace smallkv {

    DBStatus FooterBuilder::add(const OffsetInfo &meta_block_offset_info,
                                const OffsetInfo &index_block_offset_info) {
        auto first = meta_block_offset_info;
        auto second = index_block_offset_info;
        utils::PutFixed64(_data, *reinterpret_cast<uint64_t *>(&first));
        utils::PutFixed64(_data, *reinterpret_cast<uint64_t *>(&second));
        return Status::Success;
    }
}