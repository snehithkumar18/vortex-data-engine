#include "tests/test_framework.h"
#include "vde/stream/stream_manager.h"

TEST(stream_manager_init) {
    vde::StreamManager mgr(16);
    ASSERT_EQ(mgr.active_session_count(), 0u);
}

RUN_ALL_TESTS()
