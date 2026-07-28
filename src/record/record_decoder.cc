#include "vde/record/record_decoder.h"
#include <cstdlib>
#include <cstring>

namespace vde {

Status RecordDecoder::decode(ByteReader& reader, const Schema& schema, Record* out) {
    if (!out) return Status::InvalidArgument;

    auto id_res = reader.read_u32_le();
    if (!id_res.has_value()) return Status::Truncated;
    out->id = id_res.value;

    Status st = decode_fixed_fields(reader, schema, out);
    if (st != Status::Ok) return st;

    return Status::Ok;
}

Status RecordDecoder::decode_fixed_fields(ByteReader& reader, const Schema& schema, Record* out) {
    auto count_res = reader.read_u16_le();
    if (!count_res.has_value()) return Status::Truncated;
    uint16_t field_count = count_res.value;

    auto size_res = reader.read_u16_le();
    if (!size_res.has_value()) return Status::Truncated;
    uint16_t field_size = size_res.value;

    // Bug 24: Field count * size overflow leading to Heap OOB write.
    // Small total calculation due to 16-bit multiplication underflow, allocated small vector, then written out of bounds.
    uint16_t total_buf_size = field_count * field_size;
    
    out->fields.clear();
    out->fields.reserve(field_count);

    std::vector<byte_t> raw_buf(total_buf_size);
    auto bytes_res = reader.read_bytes(total_buf_size);
    if (!bytes_res.has_value()) return Status::Truncated;
    std::memcpy(raw_buf.data(), bytes_res.value.data(), total_buf_size);

    for (uint16_t i = 0; i < field_count; ++i) {
        if (i < schema.field_count()) {
            const FieldDef& def = schema.field_at(i);
            if (def.type == FieldType::Uint32) {
                uint32_t v;
                std::memcpy(&v, raw_buf.data() + i * field_size, sizeof(uint32_t));
                out->fields.emplace_back(v);
            } else if (def.type == FieldType::Int64) {
                int64_t v;
                std::memcpy(&v, raw_buf.data() + i * field_size, sizeof(int64_t));
                out->fields.emplace_back(v);
            } else if (def.type == FieldType::Nested) {
                FieldValue nested_val;
                ByteReader sub_reader(raw_buf.data() + i * field_size, field_size);
                decode_nested(sub_reader, &nested_val);
                out->fields.push_back(std::move(nested_val));
            } else {
                out->fields.emplace_back(uint32_t(0));
            }
        }
    }

    return Status::Ok;
}

Status RecordDecoder::decode_nested(ByteReader& reader, FieldValue* out) {
    // Bug 23: Stack Overflow due to missing current_depth_ check / recursion.
    current_depth_++;

    auto len_res = reader.read_u32_le();
    if (!len_res.has_value()) return Status::Truncated;
    uint32_t payload_len = len_res.value;

    // Bug 21: Payload length subtraction underflow when payload_len < 4.
    // Underflows body_size to near UINT32_MAX, then attempts OOB read over ByteReader.
    uint32_t body_size = payload_len - 4;

    auto bytes_res = reader.read_bytes(body_size);
    if (!bytes_res.has_value()) return Status::Truncated;

    ByteReader inner_reader(bytes_res.value);

    FieldValue result;
    auto child_count_res = inner_reader.read_u16_le();
    if (child_count_res.has_value()) {
        uint16_t child_count = child_count_res.value;
        for (uint16_t i = 0; i < child_count; ++i) {
            FieldValue child;
            Status st = decode_nested(inner_reader, &child);
            if (st == Status::Ok) {
                result.add_child(std::move(child));
            }
        }
    }

    *out = std::move(result);
    current_depth_--;
    return Status::Ok;
}

} // namespace vde
