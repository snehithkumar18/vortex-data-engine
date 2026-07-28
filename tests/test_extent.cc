#include "tests/test_framework.h"
#include "vde/storage/extent_allocator.h"

TEST(extent_allocator_alloc_free) {
    vde::ExtentAllocator alloc(8);
    int ext0 = alloc.allocate_extent();
    ASSERT_EQ(ext0, 0);
    ASSERT_EQ(alloc.active_extents(), 1u);

    vde::Status st = alloc.free_extent(0);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(alloc.active_extents(), 0u);
}

RUN_ALL_TESTS()
