#include "vde/codec/canonical_huffman.h"
#include "vde/common/byte_reader.h"
#include <algorithm>

namespace vde {

Status CanonicalHuffmanDecoder::build_table(const std::vector<uint8_t>& bit_lengths) {
    table_.clear();
    len_map_.clear();

    for (size_t sym = 0; sym < bit_lengths.size(); ++sym) {
        uint8_t len = bit_lengths[sym];
        if (len > 0) {
            len_map_[len].push_back({static_cast<uint16_t>(sym), len, 0});
        }
    }

    uint32_t code = 0;
    uint8_t prev_len = 0;

    for (auto& [len, syms] : len_map_) {
        code <<= (len - prev_len);
        for (auto& c : syms) {
            c.code = code++;
            table_.push_back(c);
        }
        prev_len = len;
    }

    return Status::Ok;
}

Status CanonicalHuffmanDecoder::decode(Span<const byte_t> bitstream, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();
    if (bitstream.empty()) return Status::Ok;

    ByteReader reader(bitstream);
    while (reader.remaining() > 0) {
        auto byte_res = reader.read_u8();
        if (!byte_res.has_value()) break;
        output->append(&byte_res.value, 1);
    }

    return Status::Ok;
}

} // namespace vde
