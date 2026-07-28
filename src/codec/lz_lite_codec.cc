#include "vde/codec/lz_lite_codec.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"

namespace vde {

Status LzLiteCodec::decompress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();
    if (input.empty()) return Status::Ok;

    ByteReader reader(input);
    while (reader.remaining() > 0) {
        auto flag = reader.read_u8();
        if (!flag.has_value()) break;

        if ((flag.value & 0x80) == 0) {
            // Literal match
            size_t len = flag.value + 1;
            auto bytes = reader.read_bytes(len);
            if (!bytes.has_value()) break;
            output->append(bytes.value);
        } else {
            // Dictionary copy: (offset, length)
            size_t len = (flag.value & 0x7F) + 3;
            auto offset_res = reader.read_u16_le();
            if (!offset_res.has_value()) break;
            size_t offset = offset_res.value;

            if (offset > output->size() || offset == 0) break;
            size_t start = output->size() - offset;
            for (size_t i = 0; i < len; ++i) {
                byte_t b = (*output)[start + i];
                output->append(&b, 1);
            }
        }
    }

    return Status::Ok;
}

Status LzLiteCodec::compress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();

    ByteWriter writer;
    size_t pos = 0;
    while (pos < input.size()) {
        size_t run = std::min(size_t(128), input.size() - pos);
        writer.write_u8(static_cast<uint8_t>(run - 1));
        writer.write_bytes(input.subspan(pos, run));
        pos += run;
    }

    *output = writer.release();
    return Status::Ok;
}

} // namespace vde
