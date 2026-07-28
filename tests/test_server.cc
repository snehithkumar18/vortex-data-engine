#include "tests/test_framework.h"
#include "vde/net/vde_server.h"

TEST(vde_server_start_stop) {
    vde::VdeServerDaemon server("127.0.0.1", 9091);
    vde::Status st = server.start();
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_TRUE(server.is_running());

    server.stop();
    ASSERT_FALSE(server.is_running());
}

RUN_ALL_TESTS()
