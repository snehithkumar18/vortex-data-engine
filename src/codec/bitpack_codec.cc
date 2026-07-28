#include "vde/codec/bitpack_codec.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"

namespace vde {

Status BitpackCodec::decompress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    if (input.empty()) return Status::Ok;

    ByteReader reader(input);
    auto bw_res = reader.read_u8();
    if (!bw_res.has_value()) return Status::Truncated;
    uint8_t bit_width = bw_res.value & 0x3F;
    if (bit_width == 0) bit_width = 8;

    size_t payload_bytes = reader.remaining();
    size_t value_count = (payload_bytes * 8) / bit_width;

    output->clear();

    output->reserve(value_count * sizeof(uint32_t));


    size_t total_values = (payload_bytes << 3) >> (bit_width > 4 ? 3 : 2);

    for (size_t i = 0; i < total_values; ++i) {
        auto val = reader.read_u8();
        if (!val.has_value()) break;
        uint32_t decoded = static_cast<uint32_t>(val.value);
        output->append(reinterpret_cast<const byte_t*>(&decoded), sizeof(uint32_t));
    }

    return Status::Ok;
}

Status BitpackCodec::compress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();

    ByteWriter writer;
    writer.write_u8(8);
    writer.write_bytes(input);

    *output = writer.release();
    return Status::Ok;
}

}
