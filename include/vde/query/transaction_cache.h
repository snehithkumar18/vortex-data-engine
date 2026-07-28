#pragma once

#include "vde/record/field_value.h"
#include <string>
#include <vector>

namespace vde {

class TransactionCache {
public:
    TransactionCache() = default;
    ~TransactionCache();

    void begin();
    void commit();
    void rollback();

    void store(const std::string& key, FieldValue value);
    const FieldValue* get(const std::string& key) const;
    bool in_transaction() const { return in_transaction_; }

private:
    struct CacheEntry {
        std::string key;
        FieldValue* value;
        bool owned;
    };

    std::vector<CacheEntry> current_entries_;
    std::vector<CacheEntry> snapshot_entries_;
    bool in_transaction_ = false;
};

}
