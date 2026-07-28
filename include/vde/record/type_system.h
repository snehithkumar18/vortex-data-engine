#pragma once

#include "vde/common/types.h"
#include <string>
#include <cstdint>

namespace vde {

enum class ExtendedType : uint8_t {
    Decimal128,
    TimestampMicros,
    Uuid,
    FixedList,
    Struct
};

struct Decimal128 {
    int64_t high;
    uint64_t low;
    int8_t scale;
};

struct TimestampMicros {
    int64_t micros_since_epoch;
};

struct UuidVal {
    uint8_t bytes[16];
};

class TypeSystemConverter {
public:
    TypeSystemConverter() = default;

    static std::string decimal_to_string(const Decimal128& dec);
    static std::string timestamp_to_iso8601(const TimestampMicros& ts);
    static std::string uuid_to_string(const UuidVal& uuid);
};

} // namespace vde
