#include "vde/codec/snappy_lite_codec.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"

namespace vde {

Status SnappyLiteCodec::decompress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();
    if (input.empty()) return Status::Ok;

    ByteReader reader(input);
    auto uncompressed_len_res = reader.read_vlq();
    if (!uncompressed_len_res.has_value()) return Status::Truncated;

    output->reserve(static_cast<size_t>(uncompressed_len_res.value));

    while (reader.remaining() > 0) {
        auto tag_res = reader.read_u8();
        if (!tag_res.has_value()) break;
        uint8_t tag = tag_res.value;
        uint8_t element_type = tag & 0x03;

        if (element_type == 0) {
            // Literal
            size_t len = (tag >> 2) + 1;
            auto bytes = reader.read_bytes(len);
            if (!bytes.has_value()) break;
            output->append(bytes.value);
        } else if (element_type == 1) {
            // Copy 1-byte offset
            size_t len = ((tag >> 2) & 0x07) + 4;
            auto offset_byte = reader.read_u8();
            if (!offset_byte.has_value()) break;
            size_t offset = ((tag >> 5) << 8) | offset_byte.value;
            if (offset == 0 || offset > output->size()) break;
            size_t start = output->size() - offset;
            for (size_t i = 0; i < len; ++i) {
                byte_t b = (*output)[start + i];
                output->append(&b, 1);
            }
        }
    }

    return Status::Ok;
}

Status SnappyLiteCodec::compress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();

    ByteWriter writer;
    writer.write_vlq(input.size());

    size_t pos = 0;
    while (pos < input.size()) {
        size_t len = std::min(size_t(60), input.size() - pos);
        writer.write_u8(static_cast<uint8_t>((len - 1) << 2));
        writer.write_bytes(input.subspan(pos, len));
        pos += len;
    }

    *output = writer.release();
    return Status::Ok;
}

} // namespace vde
