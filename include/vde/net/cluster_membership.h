#pragma once

#include "vde/common/types.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace vde {

enum class NodeState {
    Alive,
    Suspect,
    Dead
};

struct NodeInfo {
    uint32_t node_id;
    std::string ip_address;
    uint16_t port;
    NodeState state;
    uint64_t heartbeat_counter;
    uint64_t last_seen_ms;
};

class ClusterMembershipManager {
public:
    explicit ClusterMembershipManager(uint32_t local_node_id);
    ~ClusterMembershipManager() = default;

    void add_node(uint32_t node_id, std::string ip, uint16_t port);
    void receive_heartbeat(uint32_t node_id, uint64_t heartbeat_counter);
    void perform_failure_check(uint64_t current_time_ms, uint64_t timeout_ms = 5000);

    const NodeInfo* get_node(uint32_t node_id) const;
    size_t alive_node_count() const;
    std::vector<uint32_t> alive_nodes() const;

private:
    uint32_t local_node_id_;
    std::unordered_map<uint32_t, NodeInfo> nodes_;
};

} // namespace vde
