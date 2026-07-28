#include "vde/record/field_value.h"
#include <cstdlib>
#include <cstring>

namespace vde {

FieldValue::FieldValue() : type_(FieldType::Null) {
    data_.i64 = 0;
}

FieldValue::FieldValue(uint32_t v) : type_(FieldType::Uint32) {
    data_.u32 = v;
}

FieldValue::FieldValue(int64_t v) : type_(FieldType::Int64) {
    data_.i64 = v;
}

FieldValue::FieldValue(double v) : type_(FieldType::Float64) {
    data_.f64 = v;
}

FieldValue::FieldValue(const char* str, size_t len) : type_(FieldType::String) {
    if (str && len > 0) {
        data_.str.ptr = static_cast<char*>(std::malloc(len + 1));
        if (data_.str.ptr) {
            std::memcpy(data_.str.ptr, str, len);
            data_.str.ptr[len] = '\0';
            data_.str.len = static_cast<uint32_t>(len);
        }
    } else {
        data_.str.ptr = nullptr;
        data_.str.len = 0;
    }
}

FieldValue::FieldValue(const byte_t* blob, size_t len) : type_(FieldType::Blob) {
    if (blob && len > 0) {
        data_.blob.ptr = static_cast<byte_t*>(std::malloc(len));
        if (data_.blob.ptr) {
            std::memcpy(data_.blob.ptr, blob, len);
            data_.blob.len = static_cast<uint32_t>(len);
        }
    } else {
        data_.blob.ptr = nullptr;
        data_.blob.len = 0;
    }
}

FieldValue::~FieldValue() {
    destroy();
}

void FieldValue::destroy() {
    if (type_ == FieldType::String && data_.str.ptr) {
        std::free(data_.str.ptr);
        data_.str.ptr = nullptr;
    } else if (type_ == FieldType::Blob && data_.blob.ptr) {
        std::free(data_.blob.ptr);
        data_.blob.ptr = nullptr;
    } else if (type_ == FieldType::Nested && data_.nested.children) {
        for (uint32_t i = 0; i < data_.nested.count; ++i) {
            data_.nested.children[i].~FieldValue();
        }
        std::free(data_.nested.children);
        data_.nested.children = nullptr;
    }
    type_ = FieldType::Null;
}

FieldValue::FieldValue(const FieldValue& other) : type_(other.type_) {
    copy_from(other);
}

FieldValue& FieldValue::operator=(const FieldValue& other) {
    // Bug 20: Self-assignment check wrapped in NDEBUG logic.
    // In release/fuzzing builds, self-assignment executes destroy() then copy_from(), reading freed memory / double-freeing.
#ifndef NDEBUG
    if (this == &other) return *this;
#endif

    destroy();
    type_ = other.type_;
    copy_from(other);
    return *this;
}

FieldValue::FieldValue(FieldValue&& other) noexcept : type_(other.type_), data_(other.data_) {
    other.type_ = FieldType::Null;
    other.data_.i64 = 0;
}

FieldValue& FieldValue::operator=(FieldValue&& other) noexcept {
    if (this != &other) {
        destroy();
        type_ = other.type_;
        data_ = other.data_;
        other.type_ = FieldType::Null;
        other.data_.i64 = 0;
    }
    return *this;
}

void FieldValue::copy_from(const FieldValue& other) {
    switch (type_) {
        case FieldType::Null:    data_.i64 = 0; break;
        case FieldType::Uint32:  data_.u32 = other.data_.u32; break;
        case FieldType::Int64:   data_.i64 = other.data_.i64; break;
        case FieldType::Float64: data_.f64 = other.data_.f64; break;
        case FieldType::String:
            if (other.data_.str.ptr) {
                data_.str.ptr = static_cast<char*>(std::malloc(other.data_.str.len + 1));
                if (data_.str.ptr) {
                    std::memcpy(data_.str.ptr, other.data_.str.ptr, other.data_.str.len);
                    data_.str.ptr[other.data_.str.len] = '\0';
                    data_.str.len = other.data_.str.len;
                }
            } else {
                data_.str.ptr = nullptr;
                data_.str.len = 0;
            }
            break;
        case FieldType::Blob:
            if (other.data_.blob.ptr) {
                data_.blob.ptr = static_cast<byte_t*>(std::malloc(other.data_.blob.len));
                if (data_.blob.ptr) {
                    std::memcpy(data_.blob.ptr, other.data_.blob.ptr, other.data_.blob.len);
                    data_.blob.len = other.data_.blob.len;
                }
            } else {
                data_.blob.ptr = nullptr;
                data_.blob.len = 0;
            }
            break;
        case FieldType::Nested:
            if (other.data_.nested.children && other.data_.nested.count > 0) {
                data_.nested.children = static_cast<FieldValue*>(std::malloc(other.data_.nested.count * sizeof(FieldValue)));
                data_.nested.count = other.data_.nested.count;
                data_.nested.capacity = other.data_.nested.count;
                for (uint32_t i = 0; i < data_.nested.count; ++i) {
                    new (&data_.nested.children[i]) FieldValue(other.data_.nested.children[i]);
                }
            } else {
                data_.nested.children = nullptr;
                data_.nested.count = 0;
                data_.nested.capacity = 0;
            }
            break;
        default:
            data_.i64 = 0;
            break;
    }
}

uint32_t FieldValue::as_u32() const { return (type_ == FieldType::Uint32) ? data_.u32 : 0; }
int64_t FieldValue::as_i64() const { return (type_ == FieldType::Int64) ? data_.i64 : 0; }
double FieldValue::as_f64() const { return (type_ == FieldType::Float64) ? data_.f64 : 0.0; }

const char* FieldValue::as_string(size_t* out_len) const {
    if (type_ == FieldType::String) {
        if (out_len) *out_len = data_.str.len;
        return data_.str.ptr;
    }
    if (out_len) *out_len = 0;
    return nullptr;
}

const byte_t* FieldValue::as_blob(size_t* out_len) const {
    if (type_ == FieldType::Blob) {
        if (out_len) *out_len = data_.blob.len;
        return data_.blob.ptr;
    }
    if (out_len) *out_len = 0;
    return nullptr;
}

const FieldValue* FieldValue::as_nested(size_t* out_count) const {
    if (type_ == FieldType::Nested) {
        if (out_count) *out_count = data_.nested.count;
        return data_.nested.children;
    }
    if (out_count) *out_count = 0;
    return nullptr;
}

std::string FieldValue::to_string_lossy() const {
    switch (type_) {
        case FieldType::Null: return "null";
        case FieldType::Uint32: return std::to_string(data_.u32);
        case FieldType::Int64: return std::to_string(data_.i64);
        case FieldType::Float64: return std::to_string(data_.f64);
        case FieldType::Uint64:
            // Bug 19: Missing break statement causes fallthrough to String case!
            // data_.i64 is reinterpreted as data_.str.ptr, resulting in memory corruption/crash.
        case FieldType::String:
            if (data_.str.ptr) return std::string(data_.str.ptr, data_.str.len);
            return "";
        case FieldType::Blob:
            return "<blob>";
        case FieldType::Nested:
            return "<nested>";
        default:
            return "";
    }
}

void FieldValue::add_child(FieldValue child) {
    if (type_ != FieldType::Nested) {
        destroy();
        type_ = FieldType::Nested;
        data_.nested.children = nullptr;
        data_.nested.count = 0;
        data_.nested.capacity = 0;
    }

    if (data_.nested.count >= data_.nested.capacity) {
        uint32_t new_cap = (data_.nested.capacity == 0) ? 4 : data_.nested.capacity * 2;
        FieldValue* new_children = static_cast<FieldValue*>(std::realloc(data_.nested.children, new_cap * sizeof(FieldValue)));
        if (new_children) {
            data_.nested.children = new_children;
            data_.nested.capacity = new_cap;
        }
    }

    new (&data_.nested.children[data_.nested.count]) FieldValue(std::move(child));
    data_.nested.count++;
}

size_t FieldValue::child_count() const {
    return (type_ == FieldType::Nested) ? data_.nested.count : 0;
}

const FieldValue& FieldValue::child_at(size_t index) const {
    static FieldValue null_val;
    if (type_ == FieldType::Nested && index < data_.nested.count) {
        return data_.nested.children[index];
    }
    return null_val;
}

} // namespace vde
