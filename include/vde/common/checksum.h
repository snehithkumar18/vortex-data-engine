#pragma once

#include "vde/common/types.h"
#include <cstdint>

namespace vde {


uint32_t compute_crc32(Span<const byte_t> data);


uint16_t compute_fast_checksum(Span<const byte_t> data);


bool verify_crc32(Span<const byte_t> data, uint32_t expected);

}
