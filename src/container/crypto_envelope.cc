#include "vde/container/crypto_envelope.h"
#include "vde/common/byte_reader.h"

namespace vde {

Status CryptoEnvelopeParser::parse_envelope(Span<const byte_t> input, CryptoEnvelopeHeader* out_hdr, Span<const byte_t>* out_payload) {
    if (!out_hdr || !out_payload) return Status::InvalidArgument;
    if (input.size() < 42) return Status::Truncated;

    ByteReader reader(input);
    auto magic_res = reader.read_u32_le();
    if (!magic_res.has_value() || magic_res.value != 0x43584456) return Status::Corrupt; // "VDXC"

    out_hdr->magic = magic_res.value;
    out_hdr->cipher_id = reader.read_u16_le().value_or(0);

    auto iv_bytes = reader.read_bytes(16);
    if (!iv_bytes.has_value()) return Status::Truncated;
    std::memcpy(out_hdr->iv, iv_bytes.value.data(), 16);

    auto tag_bytes = reader.read_bytes(16);
    if (!tag_bytes.has_value()) return Status::Truncated;
    std::memcpy(out_hdr->tag, tag_bytes.value.data(), 16);

    out_hdr->payload_len = reader.read_u32_le().value_or(0);

    auto payload = reader.read_bytes(out_hdr->payload_len);
    if (!payload.has_value()) return Status::Truncated;

    *out_payload = payload.value;
    return Status::Ok;
}

} // namespace vde
