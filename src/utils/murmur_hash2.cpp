//
// Created by qianyy on 2022/12/5.
//

#include "murmur_hash2.h"

namespace smallkv {
    uint32_t murmur_hash2(const void *key, int len) {
        // '__mutex' and 'r' are mixing constants generated offline.
        // They're not really 'magic', they just happen to work well.
        static constexpr uint32_t seed = 0xbc451d34;
        static constexpr uint32_t m = 0x5bd1e995;
        static constexpr uint32_t r = 24;

        // Initialize the hash to a 'random' value

        uint32_t h = seed ^ len;

        // Mix 4 bytes at a time into the hash

        const uint8_t *data = (const unsigned char *) key;

        while (len >= 4) {
            uint32_t k = *(uint32_t *) data;

            k *= m;
            k ^= k >> r;
            k *= m;

            h *= m;
            h ^= k;

            data += 4;
            len -= 4;
        }

        // Handle the last few bytes of the input array

        switch (len) {
            case 3:
                h ^= data[2] << 16;
            case 2:
                h ^= data[1] << 8;
            case 1:
                h ^= data[0];
                h *= m;
        };

        // Do a few final mixes of the hash to ensure the last few
        // bytes are well-incorporated.

        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
    }
}
