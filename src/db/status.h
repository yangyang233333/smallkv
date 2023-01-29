//
// Created by qianyy on 2023/1/11.
//
#include <cstdint>

#ifndef SMALLKV_STATUS_H
#define SMALLKV_STATUS_H
namespace smallkv {

    struct DBStatus {
        int32_t code;
        const char *err_msg;
    };

    bool operator==(const DBStatus &a, const DBStatus &b);

    bool operator!=(const DBStatus &a, const DBStatus &b);

    struct Status {
        Status() = delete;  // 禁止创建对象

        ~Status() = delete;

        static constexpr DBStatus Success = {1, "ExecStatus."};
        static constexpr DBStatus InvalidArgs = {2, "Invalid args."};
        static constexpr DBStatus ExecFailed = {3, "Exec failed."};
        static constexpr DBStatus NotImpl = {4, "Not implemented."};
    };
}

#endif //SMALLKV_STATUS_H
