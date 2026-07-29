#include "tests/test_framework.h"
#include "vde/container/file_header.h"
#include "vde/container/section_table.h"
#include "vde/common/byte_writer.h"

TEST(file_header_parse_validate) {
    vde::ByteWriter writer;
    writer.write_bytes(vde::Span<const vde::byte_t>(reinterpret_cast<const vde::byte_t*>(vde::kMagicBytes), 4));
    writer.write_u8(1);
    writer.write_u8(2);
    writer.write_u16_le(0);
    writer.write_u32_le(2);
    writer.write_u64_le(1024);
    writer.write_u32_le(0x12345678);
    writer.write_u64_le(0);

    vde::OwnedBuffer buf = writer.release();
    vde::ByteReader reader(buf.span());

    vde::FileHeader header;
    vde::Status st = vde::parse_file_header(reader, &header);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(header.version_major, 1);
    ASSERT_EQ(header.version_minor, 2);
    ASSERT_EQ(header.section_count, 2u);
}

RUN_ALL_TESTS()
