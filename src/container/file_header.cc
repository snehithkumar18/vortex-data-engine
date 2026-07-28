#include "vde/container/file_header.h"
#include "vde/common/checksum.h"
#include <cstring>

namespace vde {

Status parse_file_header(ByteReader& reader, FileHeader* out) {
    if (!out) return Status::InvalidArgument;
    if (!reader.has_remaining(kFileHeaderSize)) return Status::Truncated;

    // Record the start position so we can extract raw bytes for CRC
    size_t header_start = reader.position();

    // Magic bytes
    auto magic_span = reader.read_bytes(4);
    if (!magic_span.has_value()) return Status::Truncated;
    std::memcpy(out->magic, magic_span.value.data(), 4);

    // Version
    auto major = reader.read_u8();
    auto minor = reader.read_u8();
    if (!major.has_value() || !minor.has_value()) return Status::Truncated;
    out->version_major = major.value;
    out->version_minor = minor.value;

    // Flags
    auto flags = reader.read_u16_le();
    if (!flags.has_value()) return Status::Truncated;
    out->flags = flags.value;

    // Section count
    auto section_count = reader.read_u32_le();
    if (!section_count.has_value()) return Status::Truncated;
    out->section_count = section_count.value;

    // Total file size
    auto total_size = reader.read_u64_le();
    if (!total_size.has_value()) return Status::Truncated;
    out->total_size = total_size.value;

    // Header checksum
    auto checksum = reader.read_u32_le();
    if (!checksum.has_value()) return Status::Truncated;
    out->checksum = checksum.value;

    // Reserved bytes
    auto reserved = reader.read_bytes(8);
    if (!reserved.has_value()) return Status::Truncated;
    std::memcpy(out->reserved, reserved.value.data(), 8);

    return Status::Ok;
}

Status validate_file_header(const FileHeader& header, Span<const byte_t> raw_header_bytes) {
    // Verify magic bytes
    if (std::memcmp(header.magic, kMagicBytes, 4) != 0) {
        return Status::Corrupt;
    }

    // Verify version compatibility
    if (!is_version_compatible(header.version_major, header.version_minor)) {
        return Status::Unsupported;
    }

    // Verify section count is within limits
    if (header.section_count > kMaxSectionCount) {
        return Status::InvalidArgument;
    }

    // Verify CRC-32 over the first 28 bytes of the header (everything
    // except the checksum field itself and the reserved tail).
    if (raw_header_bytes.size() >= 20) {
        Span<const byte_t> crc_region(raw_header_bytes.data(), 20);
        if (!verify_crc32(crc_region, header.checksum)) {
            return Status::Corrupt;
        }
    }

    return Status::Ok;
}

bool is_version_compatible(uint8_t major, uint8_t minor) {
    if (major != kFormatVersionMajor) return false;
    // Forward-compatible within the same major version
    (void)minor;
    return true;
}

} // namespace vde
