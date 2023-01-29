//
// Created by qianyy on 2023/1/29.
//

#include "memtable_iterator.h"

namespace smallkv {
    MemTableIterator::MemTableIterator(SkipList<std::string, std::string> *list) {
        iter_ = std::make_shared<SKIter>(list);
    }

    void MemTableIterator::MoveToFirst() { iter_->MoveToFirst(); }

    void MemTableIterator::Next() { iter_->Next(); }

    const std::string &MemTableIterator::key() { return iter_->key(); }

    const std::string &MemTableIterator::value() { return iter_->value(); }

    // 判断当前iter指向的位置是否有效
    bool MemTableIterator::Valid() { return iter_->Valid(); }

}