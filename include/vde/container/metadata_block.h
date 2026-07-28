#pragma once

#include "vde/common/types.h"
#include "vde/common/byte_reader.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace vde {

// Variant-like value stored in a metadata entry.
struct MetadataValue {
    enum class Type : uint8_t {
        Uint64  = 0,
        Int64   = 1,
        Float64 = 2,
        String  = 3,
        Bytes   = 4,
        Nested  = 5
    };

    Type type = Type::Uint64;

    union {
        uint64_t u64;
        int64_t  i64;
        double   f64;
        struct { char* ptr; uint32_t len; } str;
        struct { byte_t* ptr; uint32_t len; } bytes;
    } data = {};

    MetadataValue() : type(Type::Uint64) { data.u64 = 0; }
    ~MetadataValue();
    MetadataValue(const MetadataValue& other);
    MetadataValue& operator=(const MetadataValue& other);
    MetadataValue(MetadataValue&& other) noexcept;
    MetadataValue& operator=(MetadataValue&& other) noexcept;

private:
    void destroy();
    void copy_from(const MetadataValue& other);
};

struct MetadataEntry {
    uint16_t key_id;
    MetadataValue value;
};

// Recursive metadata tree.  Each node contains a flat list of key-value
// entries, and children are stored as nested MetadataNode objects owned
// via unique_ptr.
class MetadataNode {
public:
    MetadataNode() : parent_(nullptr) {}
    ~MetadataNode() = default;

    MetadataNode(const MetadataNode&) = delete;
    MetadataNode& operator=(const MetadataNode&) = delete;

    // Parse the metadata block from the reader.  Nested blocks are
    // parsed recursively.
    Status parse(ByteReader& reader);

    const MetadataEntry* find(uint16_t key_id) const;
    size_t entry_count() const { return entries_.size(); }
    MetadataNode* parent() const { return parent_; }

    const std::vector<MetadataEntry>& entries() const { return entries_; }
    const std::vector<std::unique_ptr<MetadataNode>>& children() const { return children_; }

private:
    Status parse_value(ByteReader& reader, MetadataValue* out);
    void inherit_parent_entries();

    std::vector<MetadataEntry> entries_;
    std::vector<std::unique_ptr<MetadataNode>> children_;
    MetadataNode* parent_;
};

} // namespace vde
