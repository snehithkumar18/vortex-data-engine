#include "vde/codec/vbyte_codec.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"

namespace vde {

Status VByteCodec::decompress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();
    ByteReader reader(input);

    while (reader.remaining() > 0) {
        auto val_res = reader.read_vlq();
        if (!val_res.has_value()) break;
        uint32_t val = static_cast<uint32_t>(val_res.value);
        output->append(reinterpret_cast<const byte_t*>(&val), sizeof(uint32_t));
    }

    return Status::Ok;
}

Status VByteCodec::compress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();

    size_t count = input.size() / sizeof(uint32_t);
    const uint32_t* vals = reinterpret_cast<const uint32_t*>(input.data());

    ByteWriter writer;
    for (size_t i = 0; i < count; ++i) {
        writer.write_vlq(vals[i]);
    }

    *output = writer.release();
    return Status::Ok;
}

} // namespace vde
