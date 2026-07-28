#pragma once

#include "vde/common/types.h"
#include "vde/common/byte_reader.h"
#include <vector>
#include <algorithm>

namespace vde {

enum class SectionType : uint16_t {
    Records    = 0,
    Compressed = 1,
    Index      = 2,
    Metadata   = 3,
    Extension  = 4
};

struct SectionEntry {
    SectionType type;
    uint16_t flags;
    uint64_t offset;
    uint64_t size;
};


class SectionTable {
public:
    SectionTable() : sorted_(false) {}


    Status parse(ByteReader& reader, uint32_t count);

    size_t count() const { return entries_.size(); }

    const SectionEntry& at(size_t index) const { return entries_[index]; }


    const SectionEntry* find_by_type(SectionType type) const;


    const SectionEntry* find_by_offset(uint64_t offset) const;


    Span<const byte_t> section_data(size_t index, Span<const byte_t> file_data) const;

    const std::vector<SectionEntry>& entries() const { return entries_; }

private:
    bool validate_offsets() const;

    std::vector<SectionEntry> entries_;
    bool sorted_;
};

}
