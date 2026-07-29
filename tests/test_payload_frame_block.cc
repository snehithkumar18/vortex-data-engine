#include "tests/test_framework.h"
#include "vde/storage/payload_frame_block.h"

TEST(payload_frame_block_insert_get) {
    vde::PayloadFrameBlock page(1);
    vde::byte_t data[] = "tuple_content";
    vde::Span<const vde::byte_t> tuple(data, 13);

    int slot_id = page.insert_tuple(tuple);
    ASSERT_NE(slot_id, -1);

    vde::OwnedBuffer out;
    vde::Status st = page.get_tuple(static_cast<uint16_t>(slot_id), &out);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(out.size(), 13u);
}

RUN_ALL_TESTS()
