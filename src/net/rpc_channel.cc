#include "vde/net/rpc_channel.h"

namespace vde {

void RpcChannel::register_handler(uint16_t msg_type, RpcHandler handler) {
    handlers_[msg_type] = std::move(handler);
}

OwnedBuffer RpcChannel::process_incoming_raw(Span<const byte_t> wire_data) {
    if (wire_data.size() < 14) return OwnedBuffer();

    ByteReader reader(wire_data);
    auto magic = reader.read_u32_le();
    if (!magic.has_value() || magic.value != 0x01435052) return OwnedBuffer(); // "RPC\x01"

    uint16_t msg_type = reader.read_u16_le().value_or(0);
    uint32_t req_id = reader.read_u32_le().value_or(0);
    uint32_t len = reader.read_u32_le().value_or(0);

    auto payload_res = reader.read_bytes(len);
    if (!payload_res.has_value()) return OwnedBuffer();

    auto it = handlers_.find(msg_type);
    if (it != handlers_.end()) {
        OwnedBuffer response_payload = it->second(payload_res.value);

        ByteWriter writer;
        writer.write_u32_le(0x01435052);
        writer.write_u16_le(msg_type);
        writer.write_u32_le(req_id);
        writer.write_u32_le(static_cast<uint32_t>(response_payload.size()));
        writer.write_bytes(response_payload.span());
        return writer.release();
    }

    return OwnedBuffer();
}

} // namespace vde
