#pragma once

#include "vde/common/types.h"
#include "vde/common/byte_reader.h"
#include "vde/container/file_header.h"
#include "vde/container/section_table.h"
#include "vde/container/metadata_block.h"
#include <memory>

namespace vde {

class ContainerReader {
public:
    ContainerReader() = default;
    ~ContainerReader() = default;

    ContainerReader(const ContainerReader&) = delete;
    ContainerReader& operator=(const ContainerReader&) = delete;

    Status open(Span<const byte_t> data);

    const FileHeader& header() const { return header_; }
    const SectionTable& sections() const { return sections_; }

    Result<Span<const byte_t>> read_section(size_t index) const;
    Result<std::unique_ptr<MetadataNode>> read_metadata() const;

    Status dispatch_section(size_t index, void* context);

private:
    Status compatibility_rollback();

    FileHeader header_{};
    SectionTable sections_{};
    Span<const byte_t> data_{};

    using SectionHandler = Status(*)(Span<const byte_t>, void*);
    SectionHandler handlers_[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
};

} // namespace vde
