#include "vde/codec/for_codec.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"
#include <algorithm>

namespace vde {

Status ForCodec::decompress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    if (input.empty()) return Status::Ok;

    ByteReader reader(input);
    auto min_res = reader.read_u32_le();
    if (!min_res.has_value()) return Status::Truncated;
    uint32_t min_val = min_res.value;

    auto bits_res = reader.read_u8();
    if (!bits_res.has_value()) return Status::Truncated;
    uint8_t bit_width = bits_res.value;

    output->clear();
    while (reader.remaining() > 0) {
        auto val = reader.read_vlq();
        if (!val.has_value()) break;
        uint32_t decoded = static_cast<uint32_t>(val.value + min_val);
        output->append(reinterpret_cast<const byte_t*>(&decoded), sizeof(uint32_t));
    }

    return Status::Ok;
}

Status ForCodec::compress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();
    if (input.empty()) return Status::Ok;

    size_t count = input.size() / sizeof(uint32_t);
    const uint32_t* vals = reinterpret_cast<const uint32_t*>(input.data());

    uint32_t min_val = vals[0];
    for (size_t i = 1; i < count; ++i) {
        if (vals[i] < min_val) min_val = vals[i];
    }

    ByteWriter writer;
    writer.write_u32_le(min_val);
    writer.write_u8(32); // bit width

    for (size_t i = 0; i < count; ++i) {
        writer.write_vlq(vals[i] - min_val);
    }

    *output = writer.release();
    return Status::Ok;
}

} // namespace vde
