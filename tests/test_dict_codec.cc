#include "tests/test_framework.h"
#include "vde/codec/dictionary_codec.h"

TEST(dictionary_codec_roundtrip) {
    vde::DictionaryCodec codec;
    vde::byte_t data[] = "dict_test_payload";
    vde::Span<const vde::byte_t> input(data, 17);

    vde::OwnedBuffer compressed;
    vde::Status st = codec.compress(input, &compressed);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));

    vde::OwnedBuffer decompressed;
    st = codec.decompress(compressed.span(), &decompressed);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(decompressed.size(), 17u);
}

RUN_ALL_TESTS()
