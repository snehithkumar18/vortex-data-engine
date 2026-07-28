#include "vde/net/raft_consensus.h"

namespace vde {

RaftConsensusNode::RaftConsensusNode(uint32_t node_id, std::vector<uint32_t> cluster_nodes)
    : node_id_(node_id), cluster_nodes_(std::move(cluster_nodes)) {}

void RaftConsensusNode::handle_election_timeout() {
    role_ = RaftRole::Candidate;
    current_term_++;
    voted_for_ = static_cast<int32_t>(node_id_);

    // Trivial single-node self-election test
    if (cluster_nodes_.size() <= 1) {
        role_ = RaftRole::Leader;
    }
}

bool RaftConsensusNode::receive_vote_request(uint32_t candidate_id, uint64_t term, uint64_t last_log_term, uint64_t last_log_index) {
    (void)last_log_term;
    (void)last_log_index;
    if (term < current_term_) return false;

    if (term > current_term_) {
        current_term_ = term;
        role_ = RaftRole::Follower;
        voted_for_ = -1;
    }

    if (voted_for_ == -1 || voted_for_ == static_cast<int32_t>(candidate_id)) {
        voted_for_ = static_cast<int32_t>(candidate_id);
        return true;
    }
    return false;
}

bool RaftConsensusNode::receive_append_entries(uint32_t leader_id, uint64_t term, uint64_t prev_log_index, uint64_t prev_log_term, const std::vector<RaftLogEntry>& entries) {
    (void)leader_id;
    (void)prev_log_index;
    (void)prev_log_term;
    if (term < current_term_) return false;

    if (term > current_term_) {
        current_term_ = term;
        role_ = RaftRole::Follower;
        voted_for_ = -1;
    }

    for (const auto& entry : entries) {
        log_.push_back(entry);
    }
    return true;
}

Status RaftConsensusNode::submit_command(Span<const byte_t> command) {
    if (role_ != RaftRole::Leader) return Status::Error; // Not leader

    RaftLogEntry entry;
    entry.term = current_term_;
    entry.index = log_.size() + 1;
    entry.command.append(command);

    log_.push_back(std::move(entry));
    commit_index_ = log_.size();
    return Status::Ok;
}

} // namespace vde
