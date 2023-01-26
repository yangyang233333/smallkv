//
// Created by qianyy on 2023/1/15.
//

#include "codec.h"

namespace smallkv::utils {
    void EncodeFixed8(char *buf, uint8_t val) {
        buf[0] = val & 0xff;
    }

    void EncodeFixed32(char *buf, uint32_t val) {
        buf[0] = val & 0xff;
        buf[1] = (val >> 8) & 0xff;
        buf[2] = (val >> 16) & 0xff;
        buf[3] = (val >> 24) & 0xff;
    }

    void EncodeFixed64(char *buf, uint64_t val) {
        buf[0] = val & 0xff;
        buf[1] = (val >> 8) & 0xff;
        buf[2] = (val >> 16) & 0xff;
        buf[3] = (val >> 24) & 0xff;
        buf[4] = (val >> 32) & 0xff;
        buf[5] = (val >> 40) & 0xff;
        buf[6] = (val >> 48) & 0xff;
        buf[7] = (val >> 56) & 0xff;
    }

    uint8_t DecodeFixed8(const char *data) {
        return *reinterpret_cast<const uint8_t*>(data);
    }

    uint32_t DecodeFixed32(const char *data) {
        auto _data = reinterpret_cast<const uint8_t *>(data);
        return static_cast<uint32_t>(_data[0]) |
               (static_cast<uint32_t>(_data[1]) << 8) |
               (static_cast<uint32_t>(_data[2]) << 16) |
               (static_cast<uint32_t>(_data[3]) << 24);
    }

    uint64_t DecodeFixed64(const char *data) {
        auto _data = reinterpret_cast<const uint8_t *>(data);
        return static_cast<uint64_t>(_data[0]) |
               (static_cast<uint64_t>(_data[1]) << 8) |
               (static_cast<uint64_t>(_data[2]) << 16) |
               (static_cast<uint64_t>(_data[3]) << 24) |
               (static_cast<uint64_t>(_data[4]) << 32) |
               (static_cast<uint64_t>(_data[5]) << 40) |
               (static_cast<uint64_t>(_data[6]) << 48) |
               (static_cast<uint64_t>(_data[7]) << 56);
    }

}