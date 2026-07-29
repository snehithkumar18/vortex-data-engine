#include "tests/test_framework.h"
#include "vde/telemetry_registry/system_telemetry_registrys.h"

TEST(system_telemetry_registrys_build) {
    auto batch = vde::SystemTelemetryRegistrys::build_sys_tables_batch();
    ASSERT_EQ(batch.record_count(), 0u);
}

RUN_ALL_TESTS()
