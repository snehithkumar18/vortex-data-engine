#pragma once

#include "vde/common/types.h"
#include "vde/common/byte_reader.h"
#include <cstdint>

namespace vde {

struct FileHeader {
    uint8_t magic[4];
    uint8_t version_major;
    uint8_t version_minor;
    uint16_t flags;
    uint32_t section_count;
    uint64_t total_size;
    uint32_t checksum;
    uint8_t reserved[8];
};


Status parse_file_header(ByteReader& reader, FileHeader* out);


Status validate_file_header(const FileHeader& header, Span<const byte_t> raw_header_bytes);


bool is_version_compatible(uint8_t major, uint8_t minor);

}
