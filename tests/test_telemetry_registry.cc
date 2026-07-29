#include "tests/test_framework.h"
#include "vde/telemetry_registry/telemetry_registry.h"

TEST(telemetry_registry_create_drop_table) {
    vde::TelemetryRegistry& cat = vde::TelemetryRegistry::instance();
    cat.clear();

    vde::Schema schema;
    vde::Status st = cat.create_table("users", schema);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_TRUE(cat.has_table("users"));

    st = cat.drop_table("users");
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_FALSE(cat.has_table("users"));
}

RUN_ALL_TESTS()
