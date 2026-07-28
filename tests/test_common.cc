#include "tests/test_framework.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"
#include "vde/common/arena.h"
#include "vde/common/checksum.h"

TEST(byte_reader_writer_roundtrip) {
    vde::ByteWriter writer;
    writer.write_u8(0x42);
    writer.write_u16_le(0x1234);
    writer.write_u32_le(0x87654321);
    writer.write_vlq(123456789);

    vde::OwnedBuffer buf = writer.release();
    vde::ByteReader reader(buf.span());

    ASSERT_EQ(reader.read_u8().value, 0x42);
    ASSERT_EQ(reader.read_u16_le().value, 0x1234);
    ASSERT_EQ(reader.read_u32_le().value, 0x87654321);
    ASSERT_EQ(reader.read_vlq().value, 123456789u);
}

TEST(arena_basic_allocation) {
    vde::Arena arena(1024);
    void* p1 = arena.allocate(64);
    void* p2 = arena.allocate(128);
    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    ASSERT_NE(p1, p2);
}

TEST(checksum_crc32) {
    vde::byte_t data[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    vde::Span<const vde::byte_t> span(data, 9);
    uint32_t crc = vde::compute_crc32(span);
    ASSERT_EQ(crc, 0xCBF43926u);
}

RUN_ALL_TESTS()
