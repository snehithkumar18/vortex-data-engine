#include "tests/test_framework.h"
#include "vde/net/cluster_membership.h"

TEST(cluster_membership_heartbeat) {
    vde::ClusterMembershipManager mgr(1);
    mgr.add_node(2, "127.0.0.1", 8080);
    ASSERT_EQ(mgr.alive_node_count(), 1u);

    mgr.receive_heartbeat(2, 5);
    const auto* info = mgr.get_node(2);
    ASSERT_NE(info, nullptr);
    ASSERT_EQ(info->heartbeat_counter, 5u);
}

RUN_ALL_TESTS()
