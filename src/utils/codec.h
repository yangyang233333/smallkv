//
// Created by qianyy on 2023/1/15.
//
#include <string>
#include <cstdint>

#ifndef SMALLKV_CODEC_H
#define SMALLKV_CODEC_H
namespace smallkv::utils {
    // 编解码
    void EncodeFixed8(char *buf, uint8_t val);

    void EncodeFixed32(char *buf, uint32_t val);

    void EncodeFixed64(char *buf, uint64_t val);

    uint8_t DecodeFixed8(const char *data);

    uint32_t DecodeFixed32(const char *data);

    uint64_t DecodeFixed64(const char *data);

    inline void PutFixed8(std::string &dst, uint8_t val) {
        char buf[sizeof(val)];
        EncodeFixed8(buf, val);
        dst.append(buf, sizeof(val));
    }

    inline void PutFixed32(std::string &dst, uint32_t val) {
        char buf[sizeof(val)];
        EncodeFixed32(buf, val);
        dst.append(buf, sizeof(val));
    }

    inline void PutFixed64(std::string &dst, uint64_t val) {
        char buf[sizeof(val)];
        EncodeFixed64(buf, val);
        dst.append(buf, sizeof(val));
    }

}
#endif //SMALLKV_CODEC_H
