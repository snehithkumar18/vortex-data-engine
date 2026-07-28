#include "vde/pipeline/pipeline.h"
#include "vde/codec/codec.h"

namespace vde {

Pipeline::Pipeline(const PipelineConfig& config)
    : config_(config), stream_mgr_(config.max_stream_sessions) {}

Status Pipeline::process(Span<const byte_t> vdx_data) {
    Status st = container_.open(vdx_data);
    if (st != Status::Ok) return st;

    for (size_t i = 0; i < container_.sections().count(); ++i) {
        st = process_section(i);
        if (st != Status::Ok) break;
    }
    return Status::Ok;
}

Status Pipeline::process_section(size_t section_index) {
    auto sec_res = container_.read_section(section_index);
    if (!sec_res.has_value()) return sec_res.status;

    Span<const byte_t> data = sec_res.value;
    const SectionEntry& entry = container_.sections().at(section_index);

    switch (entry.type) {
        case SectionType::Records:
            return process_record_section(data);
        case SectionType::Compressed:
            return process_compressed_section(data);
        case SectionType::Extension:
            return process_stream_section(data);
        default:
            return Status::Ok;
    }
}

Status Pipeline::process_record_section(Span<const byte_t> data) {
    ByteReader reader(data);
    Schema schema;
    Status st = schema.parse(reader);
    if (st != Status::Ok) return st;

    RecordDecoder decoder;
    Record record;
    st = decoder.decode(reader, schema, &record);
    if (st == Status::Ok) {
        records_.add_record(std::move(record));
    }
    return Status::Ok;
}

Status Pipeline::process_compressed_section(Span<const byte_t> data) {
    if (data.empty()) return Status::Ok;
    uint8_t codec_id = data[0];
    Span<const byte_t> payload = data.subspan(1);

    auto codec = create_codec(codec_id);
    if (!codec) return Status::Unsupported;

    OwnedBuffer decompressed;
    Status st = codec->decompress(payload, &decompressed);
    if (st == Status::Ok) {
        return process_record_section(decompressed.span());
    }
    return st;
}

Status Pipeline::process_stream_section(Span<const byte_t> data) {
    ByteReader reader(data);
    Fragment frag;
    Status st = parse_fragment(reader, &frag);
    if (st == Status::Ok) {
        return stream_mgr_.process_fragment(frag);
    }
    return st;
}

} // namespace vde
