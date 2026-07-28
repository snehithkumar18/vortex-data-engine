#include "vde/record/type_system.h"
#include <cstdio>

namespace vde {

std::string TypeSystemConverter::decimal_to_string(const Decimal128& dec) {
    return std::to_string(dec.high) + std::to_string(dec.low);
}

std::string TypeSystemConverter::timestamp_to_iso8601(const TimestampMicros& ts) {
    return std::to_string(ts.micros_since_epoch) + "us";
}

std::string TypeSystemConverter::uuid_to_string(const UuidVal& uuid) {
    char buf[37];
    std::snprintf(buf, sizeof(buf), "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid.bytes[0], uuid.bytes[1], uuid.bytes[2], uuid.bytes[3],
        uuid.bytes[4], uuid.bytes[5], uuid.bytes[6], uuid.bytes[7],
        uuid.bytes[8], uuid.bytes[9], uuid.bytes[10], uuid.bytes[11],
        uuid.bytes[12], uuid.bytes[13], uuid.bytes[14], uuid.bytes[15]);
    return std::string(buf);
}

}
