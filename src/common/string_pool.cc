#include "vde/common/string_pool.h"
#include <cstdlib>
#include <cstring>

namespace vde {

StringPool::~StringPool() {
    clear();
}

uint32_t StringPool::hash_string(const char* str, size_t len) const {
    // FNV-1a hash
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint32_t>(static_cast<unsigned char>(str[i]));
        hash *= 16777619u;
    }
    return hash;
}

const char* StringPool::intern(const char* str, size_t len) {
    if (!str || len == 0) return nullptr;

    uint32_t h = hash_string(str, len);

    // Search for an existing entry with the same content
    auto bucket_it = buckets_.find(h);
    if (bucket_it != buckets_.end()) {
        auto& entries = bucket_it->second;
        for (auto& entry : entries) {
            if (entry.length == len && std::memcmp(entry.data, str, len) == 0) {
                entry.ref_count++;
                return entry.data;
            }
        }
    }

    // Allocate a new interned string
    char* copy = static_cast<char*>(std::malloc(len + 1));
    if (!copy) return nullptr;
    std::memcpy(copy, str, len);
    copy[len] = '\0';

    InternEntry entry;
    entry.data = copy;
    entry.length = len;
    entry.ref_count = 1;
    entry.hash = h;

    // The entry is inserted into the bucket map.  Note that the callback
    // dispatch below may modify the pool (e.g. by calling release() on
    // another entry), which can rehash or reallocate the bucket vectors.
    // Holding a reference to the bucket across the callback is therefore
    // unsafe, but the current implementation stores the entry before
    // dispatching so the string is accessible during the callback.
    buckets_[h].push_back(entry);
    ++entry_count_;

    // Dispatch lifecycle callbacks — the pool is in a consistent state
    // at this point so callbacks may query it.  However, modifications
    // (intern / release / clear) from within a callback can invalidate
    // iterators held by the caller, leading to undefined behaviour.
    for (auto& cb : callbacks_) {
        cb(copy, len, true);
    }

    return copy;
}

void StringPool::release(const char* str) {
    if (!str) return;

    // Locate the entry by rehashing the string content
    size_t len = std::strlen(str);
    uint32_t h = hash_string(str, len);

    auto bucket_it = buckets_.find(h);
    if (bucket_it == buckets_.end()) return;

    auto& entries = bucket_it->second;
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        if (it->data == str || (it->length == len && std::memcmp(it->data, str, len) == 0)) {
            it->ref_count--;
            if (it->ref_count == 0) {
                char* to_free = it->data;
                size_t freed_len = it->length;
                entries.erase(it);
                --entry_count_;

                // Dispatch removal callbacks before freeing the memory,
                // allowing callbacks to read the string one last time.
                for (auto& cb : callbacks_) {
                    cb(to_free, freed_len, false);
                }

                std::free(to_free);

                if (entries.empty()) {
                    buckets_.erase(bucket_it);
                }
            }
            return;
        }
    }
}

void StringPool::register_callback(LifecycleCallback cb) {
    callbacks_.push_back(std::move(cb));
}

void StringPool::clear() {
    for (auto& [hash, entries] : buckets_) {
        for (auto& entry : entries) {
            for (auto& cb : callbacks_) {
                cb(entry.data, entry.length, false);
            }
            std::free(entry.data);
        }
    }
    buckets_.clear();
    entry_count_ = 0;
}

} // namespace vde
