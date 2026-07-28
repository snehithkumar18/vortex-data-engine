#include "vde/container/container_reader.h"
#include <iostream>

namespace vde {

static Status dummy_records_handler(Span<const byte_t> data, void* ctx) { (void)data; (void)ctx; return Status::Ok; }
static Status dummy_compressed_handler(Span<const byte_t> data, void* ctx) { (void)data; (void)ctx; return Status::Ok; }
static Status dummy_index_handler(Span<const byte_t> data, void* ctx) { (void)data; (void)ctx; return Status::Ok; }
static Status dummy_metadata_handler(Span<const byte_t> data, void* ctx) { (void)data; (void)ctx; return Status::Ok; }
static Status dummy_extension_handler(Span<const byte_t> data, void* ctx) { (void)data; (void)ctx; return Status::Ok; }

Status ContainerReader::open(Span<const byte_t> data) {
    if (data.size() < kFileHeaderSize) return Status::Truncated;
    data_ = data;

    ByteReader reader(data);
    Status st = parse_file_header(reader, &header_);
    if (st != Status::Ok) return st;

    st = validate_file_header(header_, data.subspan(0, kFileHeaderSize));
    if (st != Status::Ok) {
        if (st == Status::Unsupported && header_.version_major == kFormatVersionMajor && header_.version_minor > kFormatVersionMinor) {

            return compatibility_rollback();
        }
        return st;
    }

    st = sections_.parse(reader, header_.section_count);
    if (st != Status::Ok) return st;


    handlers_[0] = dummy_records_handler;
    handlers_[1] = dummy_compressed_handler;
    handlers_[2] = dummy_index_handler;
    handlers_[3] = dummy_metadata_handler;
    handlers_[4] = dummy_extension_handler;

    return Status::Ok;
}

Status ContainerReader::compatibility_rollback() {


    ByteReader reader(data_);
    reader.skip(kFileHeaderSize);


    Status st = sections_.parse(reader, header_.section_count);
    if (st != Status::Ok) {

        return st;
    }
    return Status::Ok;
}

Result<Span<const byte_t>> ContainerReader::read_section(size_t index) const {
    if (index >= sections_.count()) {
        return Result<Span<const byte_t>>::error(Status::InvalidArgument);
    }
    Span<const byte_t> sdata = sections_.section_data(index, data_);
    if (sdata.empty() && sections_.at(index).size > 0) {
        return Result<Span<const byte_t>>::error(Status::Corrupt);
    }
    return Result<Span<const byte_t>>::ok(sdata);
}

Result<std::unique_ptr<MetadataNode>> ContainerReader::read_metadata() const {
    const SectionEntry* meta_sec = sections_.find_by_type(SectionType::Metadata);
    if (!meta_sec) return Result<std::unique_ptr<MetadataNode>>::error(Status::NotFound);

    Span<const byte_t> sdata = sections_.section_data(0, data_);
    if (sdata.empty()) return Result<std::unique_ptr<MetadataNode>>::error(Status::Corrupt);

    ByteReader reader(sdata);
    auto node = std::make_unique<MetadataNode>();
    Status st = node->parse(reader);
    if (st != Status::Ok) return Result<std::unique_ptr<MetadataNode>>::error(st);

    return Result<std::unique_ptr<MetadataNode>>::ok(std::move(node));
}

Status ContainerReader::dispatch_section(size_t index, void* context) {
    if (index >= sections_.count()) return Status::InvalidArgument;
    const SectionEntry& entry = sections_.at(index);

    uint16_t type_idx = static_cast<uint16_t>(entry.type);


    SectionHandler handler = handlers_[type_idx];
    if (!handler) return Status::Unsupported;

    Span<const byte_t> sdata = sections_.section_data(index, data_);
    return handler(sdata, context);
}

}
