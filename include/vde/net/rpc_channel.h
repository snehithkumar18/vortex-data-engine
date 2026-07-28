#pragma once

#include "vde/common/types.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"
#include <functional>
#include <unordered_map>
#include <string>

namespace vde {

struct RpcMessageHeader {
    uint32_t magic; // "RPC\x01"
    uint16_t msg_type;
    uint32_t request_id;
    uint32_t payload_len;
};

using RpcHandler = std::function<OwnedBuffer(Span<const byte_t> payload)>;

class RpcChannel {
public:
    RpcChannel() = default;

    void register_handler(uint16_t msg_type, RpcHandler handler);
    OwnedBuffer process_incoming_raw(Span<const byte_t> wire_data);

private:
    std::unordered_map<uint16_t, RpcHandler> handlers_;
    uint32_t next_request_id_ = 1;
};

} // namespace vde
