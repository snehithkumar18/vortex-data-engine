#include "vde/record/schema.h"
#include <cstring>

namespace vde {

Status Schema::parse(ByteReader& reader) {
    auto count_res = reader.read_u16_le();
    if (!count_res.has_value()) return Status::Truncated;
    uint16_t count = count_res.value;

    fields_.clear();
    fields_.reserve(count);

    for (uint16_t i = 0; i < count; ++i) {
        FieldDef def{};
        auto id_res = reader.read_u16_le();
        if (!id_res.has_value()) return Status::Truncated;
        def.id = id_res.value;

        auto type_res = reader.read_u8();
        if (!type_res.has_value()) return Status::Truncated;
        def.type = static_cast<FieldType>(type_res.value);

        auto flags_res = reader.read_u16_le();
        if (!flags_res.has_value()) return Status::Truncated;
        def.flags = flags_res.value;

        auto name_bytes = reader.read_bytes(64);
        if (!name_bytes.has_value()) return Status::Truncated;
        std::memcpy(def.name, name_bytes.value.data(), 64);
        def.name[63] = '\0';

        fields_.push_back(def);
    }

    return Status::Ok;
}

const FieldDef* Schema::find_field(uint16_t id) const {
    for (const auto& f : fields_) {
        if (f.id == id) return &f;
    }
    return nullptr;
}

} // namespace vde
