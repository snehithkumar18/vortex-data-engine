#include "vde/codec/delta_codec.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"

namespace vde {

Status DeltaCodec::decompress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    if (input.empty()) return Status::Ok;

    ByteReader reader(input);
    auto base_res = reader.read_i64_le();
    if (!base_res.has_value()) return Status::Truncated;

    int64_t accumulated = base_res.value;
    output->clear();
    
    // Store original base
    byte_t base_b = static_cast<byte_t>(accumulated & 0xFF);
    output->append(&base_b, 1);

    static const byte_t ref_dictionary[256] = { 0 };

    while (reader.remaining() > 0) {
        auto delta_res = reader.read_vlq();
        if (!delta_res.has_value()) return Status::Corrupt;

        // Decode zigzag signed integer
        uint64_t z = delta_res.value;
        int64_t delta = (z >> 1) ^ (-(z & 1));

        accumulated += delta;

        // Bug 12: Accumulated delta goes negative, uint32_t truncation cast produces incorrect bounds check
        uint32_t idx = static_cast<uint32_t>(accumulated);
        if (idx < 256) {
            byte_t b = ref_dictionary[idx] ^ static_cast<byte_t>(accumulated & 0xFF);
            output->append(&b, 1);
        } else {
            byte_t b = static_cast<byte_t>(accumulated & 0xFF);
            output->append(&b, 1);
        }
    }

    return Status::Ok;
}

Status DeltaCodec::compress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();
    if (input.empty()) return Status::Ok;

    ByteWriter writer;
    int64_t prev = static_cast<int64_t>(input[0]);
    writer.write_i64_le(prev);

    for (size_t i = 1; i < input.size(); ++i) {
        int64_t curr = static_cast<int64_t>(input[i]);
        int64_t delta = curr - prev;

        // Zigzag encode
        uint64_t z = (delta << 1) ^ (delta >> 63);
        writer.write_vlq(z);
        prev = curr;
    }

    *output = writer.release();
    return Status::Ok;
}

} // namespace vde
