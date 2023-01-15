//
// Created by qianyy on 2023/1/15.
//
#include <string>
#include <cstdint>

#ifndef SMALLKV_CODEC_H
#define SMALLKV_CODEC_H
namespace smallkv::utils {
    // 编解码
    void EncodeFixed32(char *buf, int32_t val);

    void EncodeFixed64(char *buf, int64_t val);

    // todo
    void DecodeFixed32();

    // todo
    void DecodeFixed64();


    inline void PutFixed32(std::string &dst, int32_t val) {
        char buf[sizeof(val)];
        EncodeFixed32(buf, val);
        dst.append(buf, sizeof(val));
    }

    inline void PutFixed64(std::string &dst, int64_t val) {
        char buf[sizeof(val)];
        EncodeFixed64(buf, val);
        dst.append(buf, sizeof(val));
    }

}
#endif //SMALLKV_CODEC_H
