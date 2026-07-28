#include "tests/test_framework.h"
#include "vde/transaction/mvcc_storage.h"

TEST(mvcc_insert_read_version) {
    vde::MvccStorageEngine mvcc;
    vde::byte_t v1_bytes[] = "version_1";
    vde::Span<const vde::byte_t> v1(v1_bytes, 9);

    vde::Status st = mvcc.insert_tuple(100, 1, v1);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));

    vde::OwnedBuffer out;
    st = mvcc.read_tuple(105, 1, &out);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(out.size(), 9u);
}

TEST(mvcc_update_isolation) {
    vde::MvccStorageEngine mvcc;
    vde::byte_t v1_bytes[] = "v1";
    vde::byte_t v2_bytes[] = "v2_updated";

    mvcc.insert_tuple(100, 1, vde::Span<const vde::byte_t>(v1_bytes, 2));
    mvcc.update_tuple(200, 1, vde::Span<const vde::byte_t>(v2_bytes, 10));


    vde::OwnedBuffer out1;
    vde::Status st1 = mvcc.read_tuple(150, 1, &out1);
    ASSERT_EQ(static_cast<int>(st1), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(out1.size(), 2u);


    vde::OwnedBuffer out2;
    vde::Status st2 = mvcc.read_tuple(250, 1, &out2);
    ASSERT_EQ(static_cast<int>(st2), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(out2.size(), 10u);
}

RUN_ALL_TESTS()
