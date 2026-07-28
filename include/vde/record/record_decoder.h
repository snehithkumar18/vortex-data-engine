#pragma once

#include "vde/common/types.h"
#include "vde/common/byte_reader.h"
#include "vde/record/field_value.h"
#include "vde/record/schema.h"
#include <vector>

namespace vde {

struct Record {
    uint32_t id = 0;
    std::vector<FieldValue> fields;
    uint16_t schema_version = 1;
};

class RecordDecoder {
public:
    RecordDecoder() = default;

    Status decode(ByteReader& reader, const Schema& schema, Record* out);
    Status decode_nested(ByteReader& reader, FieldValue* out);
    Status decode_fixed_fields(ByteReader& reader, const Schema& schema, Record* out);

private:
    size_t current_depth_ = 0;
    static constexpr size_t kMaxDecodeDepth = 128;
};

}
