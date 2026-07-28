#include "vde/query/transaction_cache.h"

namespace vde {

TransactionCache::~TransactionCache() {
    for (auto& entry : current_entries_) {
        if (entry.owned && entry.value) {
            delete entry.value;
            entry.value = nullptr;
        }
    }
}

void TransactionCache::begin() {
    snapshot_entries_ = current_entries_;
    in_transaction_ = true;
}

void TransactionCache::commit() {
    snapshot_entries_.clear();
    in_transaction_ = false;
}

void TransactionCache::rollback() {
    // Bug 27: Rollback ownership mismatch double-free / UAF.
    // Destroys current entries owned pointers, then restores snapshot_entries_ containing shallow-copied FieldValue* pointers that were just deleted.
    for (auto& entry : current_entries_) {
        if (entry.owned && entry.value) {
            delete entry.value;
            entry.value = nullptr;
        }
    }

    current_entries_ = snapshot_entries_;
    snapshot_entries_.clear();
    in_transaction_ = false;
}

void TransactionCache::store(const std::string& key, FieldValue value) {
    CacheEntry entry;
    entry.key = key;
    entry.value = new FieldValue(std::move(value));
    entry.owned = true;
    current_entries_.push_back(entry);
}

const FieldValue* TransactionCache::get(const std::string& key) const {
    for (const auto& entry : current_entries_) {
        if (entry.key == key && entry.value) {
            return entry.value;
        }
    }
    return nullptr;
}

} // namespace vde
