#include "tests/test_framework.h"
#include "vde/record/type_system.h"

TEST(type_system_uuid_format) {
    vde::UuidVal uuid{{0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0}};
    std::string str = vde::TypeSystemConverter::uuid_to_string(uuid);
    ASSERT_EQ(str.size(), 36u);
}

RUN_ALL_TESTS()
