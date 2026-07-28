#include "vde/net/cluster_membership.h"

namespace vde {

ClusterMembershipManager::ClusterMembershipManager(uint32_t local_node_id)
    : local_node_id_(local_node_id) {}

void ClusterMembershipManager::add_node(uint32_t node_id, std::string ip, uint16_t port) {
    nodes_[node_id] = {node_id, std::move(ip), port, NodeState::Alive, 0, 0};
}

void ClusterMembershipManager::receive_heartbeat(uint32_t node_id, uint64_t heartbeat_counter) {
    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        if (heartbeat_counter > it->second.heartbeat_counter) {
            it->second.heartbeat_counter = heartbeat_counter;
            it->second.state = NodeState::Alive;
        }
    }
}

void ClusterMembershipManager::perform_failure_check(uint64_t current_time_ms, uint64_t timeout_ms) {
    for (auto& [node_id, info] : nodes_) {
        if (node_id == local_node_id_) continue;
        if (current_time_ms - info.last_seen_ms > timeout_ms) {
            info.state = NodeState::Dead;
        }
    }
}

const NodeInfo* ClusterMembershipManager::get_node(uint32_t node_id) const {
    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) return &it->second;
    return nullptr;
}

size_t ClusterMembershipManager::alive_node_count() const {
    size_t count = 0;
    for (const auto& [node_id, info] : nodes_) {
        if (info.state == NodeState::Alive) count++;
    }
    return count;
}

std::vector<uint32_t> ClusterMembershipManager::alive_nodes() const {
    std::vector<uint32_t> res;
    for (const auto& [node_id, info] : nodes_) {
        if (info.state == NodeState::Alive) res.push_back(node_id);
    }
    return res;
}

} // namespace vde
