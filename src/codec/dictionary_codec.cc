#include "vde/codec/dictionary_codec.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"

namespace vde {

Status DictionaryCodec::decompress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();
    if (input.empty()) return Status::Ok;

    ByteReader reader(input);
    auto dict_size_res = reader.read_u16_le();
    if (!dict_size_res.has_value()) return Status::Truncated;
    uint16_t dict_size = dict_size_res.value;

    std::vector<OwnedBuffer> dictionary;
    dictionary.reserve(dict_size);

    for (uint16_t i = 0; i < dict_size; ++i) {
        auto len_res = reader.read_u16_le();
        if (!len_res.has_value()) return Status::Truncated;
        auto bytes = reader.read_bytes(len_res.value);
        if (!bytes.has_value()) return Status::Truncated;
        OwnedBuffer entry;
        entry.append(bytes.value);
        dictionary.push_back(std::move(entry));
    }

    while (reader.remaining() > 0) {
        auto idx_res = reader.read_u16_le();
        if (!idx_res.has_value()) break;
        uint16_t idx = idx_res.value;
        if (idx < dictionary.size()) {
            output->append(dictionary[idx].span());
        }
    }

    return Status::Ok;
}

Status DictionaryCodec::compress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();

    ByteWriter writer;
    writer.write_u16_le(1);
    writer.write_u16_le(static_cast<uint16_t>(input.size()));
    writer.write_bytes(input);
    writer.write_u16_le(0);

    *output = writer.release();
    return Status::Ok;
}

}
