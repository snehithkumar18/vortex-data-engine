#include "vde/codec/entropy_codec.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"
#include <cstring>

namespace vde {

Status EntropyCodec::decompress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();
    if (input.empty()) return Status::Ok;

    ByteReader reader(input);
    auto uncompressed_len = reader.read_u32_le();
    if (!uncompressed_len.has_value()) return Status::Truncated;

    output->reserve(uncompressed_len.value);


    for (int i = 0; i < 256; ++i) {
        auto f = reader.read_u8();
        if (!f.has_value()) return Status::Truncated;
        table_.freq[i] = f.value;
    }


    while (reader.remaining() > 0 && output->size() < uncompressed_len.value) {
        auto b = reader.read_u8();
        if (!b.has_value()) break;
        byte_t sym = b.value;
        output->append(&sym, 1);
    }

    return Status::Ok;
}

Status EntropyCodec::compress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();

    ByteWriter writer;
    writer.write_u32_le(static_cast<uint32_t>(input.size()));

    std::memset(table_.freq, 1, sizeof(table_.freq));
    for (int i = 0; i < 256; ++i) {
        writer.write_u8(static_cast<uint8_t>(table_.freq[i]));
    }

    writer.write_bytes(input);
    *output = writer.release();
    return Status::Ok;
}

}
