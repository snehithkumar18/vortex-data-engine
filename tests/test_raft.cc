#include "tests/test_framework.h"
#include "vde/net/raft_consensus.h"

TEST(raft_node_election) {
    vde::RaftConsensusNode node(1, {1});
    node.handle_election_timeout();
    ASSERT_EQ(static_cast<int>(node.role()), static_cast<int>(vde::RaftRole::Leader));
}

RUN_ALL_TESTS()
