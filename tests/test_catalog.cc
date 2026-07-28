#include "tests/test_framework.h"
#include "vde/catalog/catalog.h"

TEST(catalog_create_drop_table) {
    vde::Catalog& cat = vde::Catalog::instance();
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
