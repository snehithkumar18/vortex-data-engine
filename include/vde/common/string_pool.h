#pragma once

#include "vde/common/types.h"
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace vde {


class StringPool {
public:
    StringPool() = default;
    ~StringPool();

    StringPool(const StringPool&) = delete;
    StringPool& operator=(const StringPool&) = delete;


    const char* intern(const char* str, size_t len);


    void release(const char* str);


    using LifecycleCallback = std::function<void(const char* str, size_t len, bool is_add)>;
    void register_callback(LifecycleCallback cb);


    void clear();

    size_t size() const { return entry_count_; }
    bool empty() const { return entry_count_ == 0; }

private:
    struct InternEntry {
        char* data;
        size_t length;
        uint32_t ref_count;
        uint32_t hash;
    };

    uint32_t hash_string(const char* str, size_t len) const;


    std::unordered_map<uint32_t, std::vector<InternEntry>> buckets_;
    std::vector<LifecycleCallback> callbacks_;
    size_t entry_count_ = 0;
};

}
