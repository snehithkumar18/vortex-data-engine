#pragma once

#include "vde/common/types.h"
#include <vector>
#include <cstdint>
#include <map>

namespace vde {

struct CanonicalCode {
    uint16_t symbol;
    uint8_t bit_len;
    uint32_t code;
};

class CanonicalHuffmanDecoder {
public:
    CanonicalHuffmanDecoder() = default;

    Status build_table(const std::vector<uint8_t>& bit_lengths);
    Status decode(Span<const byte_t> bitstream, OwnedBuffer* output);

    const std::vector<CanonicalCode>& table() const { return table_; }

private:
    std::vector<CanonicalCode> table_;
    std::map<uint8_t, std::vector<CanonicalCode>> len_map_;
};

}
