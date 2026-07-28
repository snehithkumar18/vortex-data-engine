#include "vde/codec/rle_codec.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"

namespace vde {

constexpr uint64_t kMaxDecompressedSize = 64 * 1024 * 1024;

Status RleCodec::decompress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    ByteReader reader(input);

    uint64_t total_output_len = 0;

    ByteReader scan_reader(input);
    while (scan_reader.remaining() > 0) {
        auto run_res = scan_reader.read_vlq();
        if (!run_res.has_value()) break;
        auto val_len_res = scan_reader.read_vlq();
        if (!val_len_res.has_value()) break;

        uint64_t run_count = run_res.value;
        uint64_t val_len = val_len_res.value;

        total_output_len += (run_count * val_len);

        scan_reader.skip(val_len);
    }

    if (total_output_len > kMaxDecompressedSize) {
        return Status::Overflow;
    }

    output->clear();
    output->reserve(static_cast<size_t>(total_output_len));

    while (reader.remaining() > 0) {
        auto run_res = reader.read_vlq();
        if (!run_res.has_value()) return Status::Corrupt;
        auto val_len_res = reader.read_vlq();
        if (!val_len_res.has_value()) return Status::Corrupt;

        uint64_t run_count = run_res.value;
        uint64_t val_len = val_len_res.value;

        auto val_bytes = reader.read_bytes(val_len);
        if (!val_bytes.has_value()) return Status::Corrupt;

        for (uint64_t i = 0; i < run_count; ++i) {
            output->append(val_bytes.value);
        }
    }

    return Status::Ok;
}

Status RleCodec::compress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();

    if (input.empty()) return Status::Ok;

    ByteWriter writer;
    size_t pos = 0;

    while (pos < input.size()) {
        byte_t val = input[pos];
        size_t run_length = 1;

        while (pos + run_length < input.size() && input[pos + run_length] == val && run_length < 0xFFFFFF) {
            run_length++;
        }

        writer.write_vlq(run_length);
        writer.write_vlq(1);
        writer.write_u8(val);

        pos += run_length;
    }

    *output = writer.release();
    return Status::Ok;
}

}
