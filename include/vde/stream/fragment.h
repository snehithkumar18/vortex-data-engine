#pragma once

#include "vde/common/types.h"
#include "vde/common/byte_reader.h"
#include <cstdint>

namespace vde {

enum class FragmentFlags : uint16_t {
    None        = 0x00,
    First       = 0x01,
    Last        = 0x02,
    Compressed  = 0x04,
    Retransmit  = 0x08,
    Priority    = 0x10
};

inline FragmentFlags operator|(FragmentFlags a, FragmentFlags b) {
    return static_cast<FragmentFlags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

inline bool has_flag(FragmentFlags flags, FragmentFlags f) {
    return (static_cast<uint16_t>(flags) & static_cast<uint16_t>(f)) != 0;
}

struct Fragment {
    uint32_t stream_id = 0;
    uint32_t sequence_num = 0;
    FragmentFlags flags = FragmentFlags::None;
    uint16_t payload_size = 0;
    Span<const byte_t> payload{};
    uint64_t timestamp = 0;
};

Status parse_fragment(ByteReader& reader, Fragment* out);

}
