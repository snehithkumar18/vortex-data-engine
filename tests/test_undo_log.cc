#include "tests/test_framework.h"
#include "vde/transaction/undo_log.h"

TEST(undo_log_append_rollback) {
    vde::UndoLogSegmentManager undo_mgr;
    vde::byte_t payload[] = "undo_data";
    uint64_t lsn = undo_mgr.append_undo(1, 10, 0, vde::Span<const vde::byte_t>(payload, 9));
    ASSERT_EQ(lsn, 1u);
    ASSERT_EQ(undo_mgr.segment_count(), 1u);

    vde::Status st = undo_mgr.rollback_transaction(1);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
}

RUN_ALL_TESTS()
