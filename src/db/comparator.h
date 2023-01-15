//
// Created by qianyy on 2023/1/12.
//
#include <string>

#ifndef SMALLKV_COMPARATOR_H
#define SMALLKV_COMPARATOR_H
namespace smallkv {
    class Comparator {
    public:
        Comparator() = default;

        virtual ~Comparator() = 0;

        // 比较器的名字
        virtual std::string name() = 0;

        // 比较key_a和key_b的大小
        virtual int32_t compare(const std::string &key_a, const std::string &key_b) = 0;
    };

    class ByteComparator final : public Comparator {
    public:
        ByteComparator() = default;

        ~ByteComparator() override = default;

        std::string name() override { return "ByteComparator"; }

        int32_t compare(const std::string &key_a, const std::string &key_b) override {
            return key_a.compare(key_b);
        }
    };

}
#endif //SMALLKV_COMPARATOR_H
