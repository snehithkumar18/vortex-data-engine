#pragma once

#include "vde/common/types.h"
#include <vector>
#include <string>

namespace vde {

struct WirePacketHeader {
    uint32_t magic;
    uint16_t version;
    uint16_t packet_type;
    uint32_t session_id;
    uint32_t payload_len;
    uint32_t crc32;
};

class PacketSerializer {
public:
    PacketSerializer() = default;

    OwnedBuffer serialize_packet(const WirePacketHeader& header, Span<const byte_t> payload);
    Status deserialize_packet(Span<const byte_t> wire_bytes, WirePacketHeader* out_hdr, OwnedBuffer* out_payload);
};

} // namespace vde
