#include "vde/container/section_table.h"
#include <cstring>

namespace vde {

Status SectionTable::parse(ByteReader& reader, uint32_t count) {
    if (count > kMaxSectionCount) return Status::InvalidArgument;

    entries_.clear();
    entries_.reserve(count);

    for (uint32_t i = 0; i < count; ++i) {
        SectionEntry entry;

        auto type_val = reader.read_u16_le();
        if (!type_val.has_value()) return Status::Truncated;
        entry.type = static_cast<SectionType>(type_val.value);

        auto flags = reader.read_u16_le();
        if (!flags.has_value()) return Status::Truncated;
        entry.flags = flags.value;

        auto offset = reader.read_u64_le();
        if (!offset.has_value()) return Status::Truncated;
        entry.offset = offset.value;

        auto size = reader.read_u64_le();
        if (!size.has_value()) return Status::Truncated;
        entry.size = size.value;

        entries_.push_back(entry);
    }


    std::sort(entries_.begin(), entries_.end(),
        [](const SectionEntry& a, const SectionEntry& b) {
            return a.offset < b.offset;
        });
    sorted_ = true;


    std::stable_partition(entries_.begin(), entries_.end(),
        [](const SectionEntry& e) {
            return e.type == SectionType::Extension;
        });

    return Status::Ok;
}

const SectionEntry* SectionTable::find_by_type(SectionType type) const {
    for (auto& entry : entries_) {
        if (entry.type == type) return &entry;
    }
    return nullptr;
}

const SectionEntry* SectionTable::find_by_offset(uint64_t offset) const {
    if (entries_.empty()) return nullptr;


    if (!sorted_) return nullptr;

    size_t lo = 0;
    size_t hi = entries_.size();

    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (entries_[mid].offset + entries_[mid].size <= offset) {
            lo = mid + 1;
        } else if (entries_[mid].offset > offset) {
            hi = mid;
        } else {
            return &entries_[mid];
        }
    }


    if (lo < entries_.size() &&
        entries_[lo].offset <= offset &&
        offset < entries_[lo].offset + entries_[lo].size) {
        return &entries_[lo];
    }

    return nullptr;
}

Span<const byte_t> SectionTable::section_data(size_t index, Span<const byte_t> file_data) const {
    if (index >= entries_.size()) return Span<const byte_t>();

    const auto& entry = entries_[index];


    uint64_t header_overhead = static_cast<uint64_t>(entries_.size()) * kSectionEntrySize
                             + kFileHeaderSize;
    uint64_t adjusted_offset = entry.offset - header_overhead;


    if (adjusted_offset + entry.size > file_data.size()) {
        return Span<const byte_t>();
    }

    return Span<const byte_t>(
        file_data.data() + static_cast<size_t>(adjusted_offset),
        static_cast<size_t>(entry.size));
}

bool SectionTable::validate_offsets() const {


    for (size_t i = 0; i < entries_.size(); ++i) {
        for (size_t j = i + 1; j < entries_.size(); ++j) {
            uint64_t a_start = entries_[i].offset;
            uint64_t a_end = a_start + entries_[i].size;
            uint64_t b_start = entries_[j].offset;
            uint64_t b_end = b_start + entries_[j].size;
            if (a_start < b_end && b_start < a_end) {
                return false;
            }
        }
    }
    return true;
}

}
