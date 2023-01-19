//
// Created by qianyy on 2023/1/17.
//
#include <string>
#include <memory>
#include <vector>

#ifndef SMALLKV_FILTER_BLOCK_BUILDER_H
#define SMALLKV_FILTER_BLOCK_BUILDER_H
namespace smallkv {
    class FilterPolicy;

    /*
     * 操作Filter
     *
     * */
    class FilterBlockBuilder final {
    private:
        std::shared_ptr<FilterPolicy> filterPolicy = nullptr;

        // todo: 这种写法有点拉胯(存在一定内存开销)，应该在Filter层直接提供add接口，这样就不需要保存_data了
        std::vector<std::string> _key_data;

        std::string _data; // 保存当前Filter的全部数据，用于持久化

    public:
        explicit FilterBlockBuilder(int32_t keys_num, double false_positive = 0.01);

        ~FilterBlockBuilder() = default;

        // 插入key到_data中，并不是真正的构建filter
        void add(const std::string &key);

        // 真正的创建Filter，然后将filter中位数组复制到_buffer
        void finish_filter_block();

        inline std::string_view data() {
            return _data;
        };

        inline void clear() {
            _key_data.clear();
            _data.clear();
        }

        bool exists(const std::string_view &key);

    private:
        inline void create_filter();
    };
}
#endif //SMALLKV_FILTER_BLOCK_BUILDER_H
