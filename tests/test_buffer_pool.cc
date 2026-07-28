#include "tests/test_framework.h"
#include "vde/common/buffer_pool.h"

TEST(buffer_pool_acquire_release) {
    vde::BufferPool pool(1024, 16);
    vde::byte_t* b1 = pool.acquire();
    ASSERT_NE(b1, nullptr);
    pool.release(b1);
    ASSERT_EQ(pool.available_blocks(), 1u);
}

TEST(ring_buffer_read_write) {
    vde::RingBuffer rb(128);
    vde::byte_t data[] = { 1, 2, 3, 4, 5 };
    vde::Span<const vde::byte_t> span(data, 5);

    size_t written = rb.write(span);
    ASSERT_EQ(written, 5u);

    vde::byte_t out[5] = {0};
    size_t read_bytes = rb.read(out, 5);
    ASSERT_EQ(read_bytes, 5u);
    ASSERT_EQ(out[0], 1);
    ASSERT_EQ(out[4], 5);
}

RUN_ALL_TESTS()
