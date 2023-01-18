//
// Created by qianyy on 2023/1/18.
//

#include "index_block_builder.h"
#include "../utils/codec.h"
#include "../file/file_writer.h"

namespace smallkv {

    DBStatus IndexBlockBuilder::add(const std::string &shortest_key,
                                    const OffsetInfo &offsetInfo) {
        utils::PutFixed32(_data, shortest_key.size());
        _data.append(shortest_key);
        OffsetInfo _offsetInfo = offsetInfo;
        // todo：这种写法可能有点问题，目前不太清楚
        utils::PutFixed64(_data, *reinterpret_cast<uint64_t *>(&_offsetInfo));
        return Status::Success;
    }

}