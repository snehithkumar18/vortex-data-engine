#pragma once

#include "vde/common/types.h"
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace vde {

// String interning pool — deduplicates identical strings and provides
// stable pointers for the lifetime of the pool.  Lifecycle callbacks
// are dispatched when strings are added or removed.
class StringPool {
public:
    StringPool() = default;
    ~StringPool();

    StringPool(const StringPool&) = delete;
    StringPool& operator=(const StringPool&) = delete;

    // Intern a string.  Returns a stable pointer to the interned copy.
    // If the string already exists the existing pointer is returned and
    // its reference count is incremented.
    const char* intern(const char* str, size_t len);

    // Release one reference to a previously interned string.  When the
    // reference count reaches zero the string is freed.
    void release(const char* str);

    // Lifecycle callback — invoked when a string is added to or removed
    // from the pool.  |is_add| is true for additions, false for removals.
    using LifecycleCallback = std::function<void(const char* str, size_t len, bool is_add)>;
    void register_callback(LifecycleCallback cb);

    // Remove all interned strings, firing removal callbacks for each.
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

    // Bucket map keyed by hash — collisions are stored in a per-bucket
    // vector and resolved by comparing the full string content.
    std::unordered_map<uint32_t, std::vector<InternEntry>> buckets_;
    std::vector<LifecycleCallback> callbacks_;
    size_t entry_count_ = 0;
};

} // namespace vde
