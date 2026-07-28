#include "vde/container/metadata_block.h"
#include <cstdlib>
#include <cstring>

namespace vde {

MetadataValue::~MetadataValue() {
    destroy();
}

MetadataValue::MetadataValue(const MetadataValue& other) : type(other.type) {
    copy_from(other);
}

MetadataValue& MetadataValue::operator=(const MetadataValue& other) {
    if (this != &other) {
        destroy();
        type = other.type;
        copy_from(other);
    }
    return *this;
}

MetadataValue::MetadataValue(MetadataValue&& other) noexcept : type(other.type), data(other.data) {
    other.type = Type::Uint64;
    other.data.u64 = 0;
}

MetadataValue& MetadataValue::operator=(MetadataValue&& other) noexcept {
    if (this != &other) {
        destroy();
        type = other.type;
        data = other.data;
        other.type = Type::Uint64;
        other.data.u64 = 0;
    }
    return *this;
}

void MetadataValue::destroy() {
    if (type == Type::String && data.str.ptr) {
        std::free(data.str.ptr);
        data.str.ptr = nullptr;
    } else if (type == Type::Bytes && data.bytes.ptr) {
        std::free(data.bytes.ptr);
        data.bytes.ptr = nullptr;
    }
}

void MetadataValue::copy_from(const MetadataValue& other) {
    type = other.type;
    switch (type) {
        case Type::Uint64:  data.u64 = other.data.u64; break;
        case Type::Int64:   data.i64 = other.data.i64; break;
        case Type::Float64: data.f64 = other.data.f64; break;
        case Type::String:
            if (other.data.str.ptr) {
                data.str.ptr = static_cast<char*>(std::malloc(other.data.str.len + 1));
                if (data.str.ptr) {
                    std::memcpy(data.str.ptr, other.data.str.ptr, other.data.str.len);
                    data.str.ptr[other.data.str.len] = '\0';
                    data.str.len = other.data.str.len;
                }
            } else {
                data.str.ptr = nullptr;
                data.str.len = 0;
            }
            break;
        case Type::Bytes:
            if (other.data.bytes.ptr) {
                data.bytes.ptr = static_cast<byte_t*>(std::malloc(other.data.bytes.len));
                if (data.bytes.ptr) {
                    std::memcpy(data.bytes.ptr, other.data.bytes.ptr, other.data.bytes.len);
                    data.bytes.len = other.data.bytes.len;
                }
            } else {
                data.bytes.ptr = nullptr;
                data.bytes.len = 0;
            }
            break;
        case Type::Nested:
            data.u64 = other.data.u64;
            break;
    }
}

Status MetadataNode::parse(ByteReader& reader) {
    auto count_res = reader.read_u16_le();
    if (!count_res.has_value()) return Status::Truncated;
    uint16_t count = count_res.value;

    entries_.reserve(count);

    for (uint16_t i = 0; i < count; ++i) {
        auto key_res = reader.read_u16_le();
        if (!key_res.has_value()) return Status::Truncated;
        uint16_t key = key_res.value;

        MetadataValue val;
        Status st = parse_value(reader, &val);
        if (st != Status::Ok) return st;

        if (key == kMetadataInheritKey && parent_) {


            inherit_parent_entries();
        } else {
            entries_.push_back({key, std::move(val)});
        }
    }

    return Status::Ok;
}

Status MetadataNode::parse_value(ByteReader& reader, MetadataValue* out) {
    auto type_res = reader.read_u8();
    if (!type_res.has_value()) return Status::Truncated;
    uint8_t t = type_res.value;

    switch (static_cast<MetadataValue::Type>(t)) {
        case MetadataValue::Type::Uint64: {
            auto r = reader.read_u64_le();
            if (!r.has_value()) return Status::Truncated;
            out->type = MetadataValue::Type::Uint64;
            out->data.u64 = r.value;
            break;
        }
        case MetadataValue::Type::Int64: {
            auto r = reader.read_i64_le();
            if (!r.has_value()) return Status::Truncated;
            out->type = MetadataValue::Type::Int64;
            out->data.i64 = r.value;
            break;
        }
        case MetadataValue::Type::Float64: {
            auto r = reader.read_f64_le();
            if (!r.has_value()) return Status::Truncated;
            out->type = MetadataValue::Type::Float64;
            out->data.f64 = r.value;
            break;
        }
        case MetadataValue::Type::String: {
            auto len_res = reader.read_u16_le();
            if (!len_res.has_value()) return Status::Truncated;
            uint16_t len = len_res.value;
            auto bytes = reader.read_bytes(len);
            if (!bytes.has_value()) return Status::Truncated;

            out->type = MetadataValue::Type::String;
            out->data.str.ptr = static_cast<char*>(std::malloc(len + 1));
            if (out->data.str.ptr) {
                std::memcpy(out->data.str.ptr, bytes.value.data(), len);
                out->data.str.ptr[len] = '\0';
                out->data.str.len = len;
            }
            break;
        }
        case MetadataValue::Type::Bytes: {
            auto len_res = reader.read_u32_le();
            if (!len_res.has_value()) return Status::Truncated;
            uint32_t len = len_res.value;
            auto bytes = reader.read_bytes(len);
            if (!bytes.has_value()) return Status::Truncated;

            out->type = MetadataValue::Type::Bytes;
            out->data.bytes.ptr = static_cast<byte_t*>(std::malloc(len));
            if (out->data.bytes.ptr) {
                std::memcpy(out->data.bytes.ptr, bytes.value.data(), len);
                out->data.bytes.len = len;
            }
            break;
        }
        case MetadataValue::Type::Nested: {
            out->type = MetadataValue::Type::Nested;
            auto child = std::make_unique<MetadataNode>();
            child->parent_ = this;


            Status st = child->parse(reader);
            if (st != Status::Ok) return st;

            children_.push_back(std::move(child));
            break;
        }
        default:
            return Status::Corrupt;
    }

    return Status::Ok;
}

void MetadataNode::inherit_parent_entries() {
    if (!parent_) return;
    for (const auto& entry : parent_->entries_) {
        entries_.push_back(entry);

        parent_->entries_.push_back(entry);
    }
}

const MetadataEntry* MetadataNode::find(uint16_t key_id) const {
    for (const auto& entry : entries_) {
        if (entry.key_id == key_id) return &entry;
    }
    return nullptr;
}

}
