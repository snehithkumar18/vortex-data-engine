#include "tests/test_framework.h"
#include "vde/codec/entropy_codec.h"

TEST(entropy_codec_compress_decompress) {
    vde::EntropyCodec codec;
    vde::byte_t data[] = "ans_entropy_data";
    vde::Span<const vde::byte_t> input(data, 16);

    vde::OwnedBuffer compressed;
    vde::Status st = codec.compress(input, &compressed);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));

    vde::OwnedBuffer decompressed;
    st = codec.decompress(compressed.span(), &decompressed);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(decompressed.size(), 16u);
}

RUN_ALL_TESTS()
