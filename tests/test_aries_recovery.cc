#include "tests/test_framework.h"
#include "vde/transaction/aries_recovery.h"

TEST(aries_recovery_basic) {
    vde::WriteAheadLog wal;
    vde::LogRecord r1{0, 0, 1, vde::LogRecordType::Begin, 0, 0, {}, {}};
    vde::LogRecord r2{0, 0, 1, vde::LogRecordType::Insert, 10, 0, {}, {}};
    wal.append_record(r1);
    wal.append_record(r2);

    vde::AriesRecoveryEngine recovery(&wal);
    vde::Status st = recovery.run_recovery_pass();
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(recovery.active_tx_count(), 1u);
}

RUN_ALL_TESTS()
