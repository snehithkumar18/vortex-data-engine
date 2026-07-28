#pragma once

#include "vde/common/types.h"
#include <cstdint>

namespace vde {

// Compute a CRC-32 checksum over the given byte span.
uint32_t compute_crc32(Span<const byte_t> data);

// Compute a lightweight XOR-rotate checksum.  Faster than CRC-32 but
// weaker — used only for quick integrity pre-checks.
uint16_t compute_fast_checksum(Span<const byte_t> data);

// Verify a CRC-32 checksum against an expected value.
bool verify_crc32(Span<const byte_t> data, uint32_t expected);

} // namespace vde
