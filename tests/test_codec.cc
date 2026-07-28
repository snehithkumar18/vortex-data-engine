#include "tests/test_framework.h"
#include "vde/codec/rle_codec.h"
#include "vde/codec/delta_codec.h"

TEST(rle_codec_roundtrip) {
    vde::RleCodec codec;
    vde::byte_t input_bytes[] = { 0x41, 0x41, 0x41, 0x42, 0x42, 0x43 };
    vde::Span<const vde::byte_t> input(input_bytes, 6);

    vde::OwnedBuffer compressed;
    vde::Status st = codec.compress(input, &compressed);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));

    vde::OwnedBuffer decompressed;
    st = codec.decompress(compressed.span(), &decompressed);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(decompressed.size(), 6u);
    ASSERT_EQ(decompressed[0], 0x41);
    ASSERT_EQ(decompressed[5], 0x43);
}

TEST(delta_codec_roundtrip) {
    vde::DeltaCodec codec;
    vde::byte_t input_bytes[] = { 10, 12, 15, 14, 20 };
    vde::Span<const vde::byte_t> input(input_bytes, 5);

    vde::OwnedBuffer compressed;
    vde::Status st = codec.compress(input, &compressed);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));

    vde::OwnedBuffer decompressed;
    st = codec.decompress(compressed.span(), &decompressed);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(decompressed.size(), 5u);
    ASSERT_EQ(decompressed[0], 10);
    ASSERT_EQ(decompressed[4], 20);
}

RUN_ALL_TESTS()
