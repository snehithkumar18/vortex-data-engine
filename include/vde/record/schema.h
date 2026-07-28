#pragma once

#include "vde/common/types.h"
#include "vde/common/byte_reader.h"
#include "vde/record/field_value.h"
#include <vector>
#include <string>

namespace vde {

struct FieldDef {
    uint16_t id;
    FieldType type;
    uint16_t flags;
    char name[64];
};

class Schema {
public:
    Schema() = default;

    Status parse(ByteReader& reader);

    size_t field_count() const { return fields_.size(); }
    const FieldDef& field_at(size_t index) const { return fields_[index]; }
    const FieldDef* find_field(uint16_t id) const;

private:
    std::vector<FieldDef> fields_;
    uint32_t version_ = 1;
};

} // namespace vde
