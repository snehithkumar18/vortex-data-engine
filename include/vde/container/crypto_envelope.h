#pragma once

#include "vde/common/types.h"

namespace vde {

struct CryptoEnvelopeHeader {
    uint32_t magic; // "VDXC"
    uint16_t cipher_id;
    uint8_t iv[16];
    uint8_t tag[16];
    uint32_t payload_len;
};

class CryptoEnvelopeParser {
public:
    CryptoEnvelopeParser() = default;

    Status parse_envelope(Span<const byte_t> input, CryptoEnvelopeHeader* out_hdr, Span<const byte_t>* out_payload);
};

} // namespace vde
