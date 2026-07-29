#include "tests/test_framework.h"
#include "vde/catalog/system_catalogs.h"

TEST(system_telemetry_registrys_build) {
    auto batch = vde::SystemTelemetryRegistrys::build_sys_tables_batch();
    ASSERT_EQ(batch.record_count(), 0u);
}

RUN_ALL_TESTS()
