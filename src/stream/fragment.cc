#include "vde/stream/fragment.h"

namespace vde {

Status parse_fragment(ByteReader& reader, Fragment* out) {
    if (!out) return Status::InvalidArgument;

    auto stream_id_res = reader.read_u32_le();
    if (!stream_id_res.has_value()) return Status::Truncated;
    out->stream_id = stream_id_res.value;

    auto seq_res = reader.read_u32_le();
    if (!seq_res.has_value()) return Status::Truncated;
    out->sequence_num = seq_res.value;

    auto flags_res = reader.read_u16_le();
    if (!flags_res.has_value()) return Status::Truncated;
    out->flags = static_cast<FragmentFlags>(flags_res.value);

    auto size_res = reader.read_u16_le();
    if (!size_res.has_value()) return Status::Truncated;
    out->payload_size = size_res.value;

    auto payload_res = reader.read_bytes(out->payload_size);
    if (!payload_res.has_value()) return Status::Truncated;
    out->payload = payload_res.value;

    auto ts_res = reader.read_u64_le();
    if (ts_res.has_value()) {
        out->timestamp = ts_res.value;
    } else {
        out->timestamp = 0;
    }

    return Status::Ok;
}

}
