#pragma once

#include "vde/common/types.h"
#include <cstdint>
#include <string>

namespace vde {

enum class FieldType : uint8_t {
    Null    = 0,
    Uint32  = 1,
    Int64   = 2,
    Float64 = 3,
    String  = 4,
    Blob    = 5,
    Nested  = 6,
    Bool    = 7,
    Uint64  = 8
};

class FieldValue {
public:
    FieldValue();
    explicit FieldValue(uint32_t v);
    explicit FieldValue(int64_t v);
    explicit FieldValue(double v);
    FieldValue(const char* str, size_t len);
    FieldValue(const byte_t* blob, size_t len);
    ~FieldValue();

    FieldValue(const FieldValue& other);
    FieldValue& operator=(const FieldValue& other);
    FieldValue(FieldValue&& other) noexcept;
    FieldValue& operator=(FieldValue&& other) noexcept;

    FieldType type() const { return type_; }

    uint32_t as_u32() const;
    int64_t as_i64() const;
    double as_f64() const;
    const char* as_string(size_t* out_len) const;
    const byte_t* as_blob(size_t* out_len) const;
    const FieldValue* as_nested(size_t* out_count) const;

    std::string to_string_lossy() const;

    void add_child(FieldValue child);
    size_t child_count() const;
    const FieldValue& child_at(size_t index) const;

private:
    void destroy();
    void copy_from(const FieldValue& other);

    FieldType type_;
    union {
        uint32_t u32;
        int64_t  i64;
        double   f64;
        struct { char* ptr; uint32_t len; } str;
        struct { byte_t* ptr; uint32_t len; } blob;
        struct { FieldValue* children; uint32_t count; uint32_t capacity; } nested;
    } data_;
};

} // namespace vde
