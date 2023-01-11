//
// Created by qianyy on 2023/1/11.
//

#include "status.h"

namespace smallkv {
    bool operator==(const DBStatus &a, const DBStatus &b) {
        return a.code == b.code;
    }

    bool operator!=(const DBStatus &a, const DBStatus &b) {
        return a.code != b.code;
    }

}