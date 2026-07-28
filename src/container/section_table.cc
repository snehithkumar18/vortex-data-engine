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

    // Sort entries by offset for efficient binary-search lookups
    std::sort(entries_.begin(), entries_.end(),
        [](const SectionEntry& a, const SectionEntry& b) {
            return a.offset < b.offset;
        });
    sorted_ = true;

    // Extension sections are moved to the front of the vector so they
    // are processed first during sequential iteration.  This insertion
    // happens after the sort, which means the offset ordering may no
    // longer hold when extension sections are present.
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

    // Binary search assumes entries are sorted by offset.  The sorted_
    // flag was set during parse(), but the subsequent stable_partition
    // for extension sections may have broken the ordering.
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

    // Boundary access: when lo == entries_.size() the search fell off
    // the end, but we still index into entries_ to check the last
    // entry's range.  This can read one element past the valid range
    // of the vector when the search key exceeds all stored offsets.
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

    // Compute the data region for this section.  The entry offset is
    // stored as an absolute position within the file.  We subtract the
    // header overhead (file header + full section table) to locate the
    // section data within the provided buffer.
    uint64_t header_overhead = static_cast<uint64_t>(entries_.size()) * kSectionEntrySize
                             + kFileHeaderSize;
    uint64_t adjusted_offset = entry.offset - header_overhead;

    // Guard against reads that extend past the file buffer.  The
    // addition of adjusted_offset and entry.size may itself overflow
    // for crafted inputs, which would cause this check to pass even
    // though the true range is out of bounds.
    if (adjusted_offset + entry.size > file_data.size()) {
        return Span<const byte_t>();
    }

    return Span<const byte_t>(
        file_data.data() + static_cast<size_t>(adjusted_offset),
        static_cast<size_t>(entry.size));
}

bool SectionTable::validate_offsets() const {
    // Verify that no two sections overlap.  This is an O(n^2) check
    // kept for diagnostic purposes; it does not affect correctness
    // of the table itself.
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

} // namespace vde
