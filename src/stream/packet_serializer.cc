#include "vde/stream/packet_serializer.h"
#include "vde/common/byte_writer.h"
#include "vde/common/byte_reader.h"
#include "vde/common/checksum.h"

namespace vde {

OwnedBuffer PacketSerializer::serialize_packet(const WirePacketHeader& header, Span<const byte_t> payload) {
    ByteWriter writer;
    writer.write_u32_le(0x56445850); // "VDXP"
    writer.write_u16_le(header.version);
    writer.write_u16_le(header.packet_type);
    writer.write_u32_le(header.session_id);
    writer.write_u32_le(static_cast<uint32_t>(payload.size()));

    uint32_t crc = compute_crc32(payload);
    writer.write_u32_le(crc);
    writer.write_bytes(payload);

    return writer.release();
}

Status PacketSerializer::deserialize_packet(Span<const byte_t> wire_bytes, WirePacketHeader* out_hdr, OwnedBuffer* out_payload) {
    if (!out_hdr || !out_payload) return Status::InvalidArgument;
    if (wire_bytes.size() < 20) return Status::Truncated;

    ByteReader reader(wire_bytes);
    auto magic_res = reader.read_u32_le();
    if (!magic_res.has_value() || magic_res.value != 0x56445850) return Status::Corrupt;

    out_hdr->magic = magic_res.value;
    out_hdr->version = reader.read_u16_le().value_or(0);
    out_hdr->packet_type = reader.read_u16_le().value_or(0);
    out_hdr->session_id = reader.read_u32_le().value_or(0);
    out_hdr->payload_len = reader.read_u32_le().value_or(0);
    out_hdr->crc32 = reader.read_u32_le().value_or(0);

    auto payload_bytes = reader.read_bytes(out_hdr->payload_len);
    if (!payload_bytes.has_value()) return Status::Truncated;

    out_payload->clear();
    out_payload->append(payload_bytes.value);

    return Status::Ok;
}

} // namespace vde
