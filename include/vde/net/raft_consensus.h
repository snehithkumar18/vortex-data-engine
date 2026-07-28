#pragma once

#include "vde/common/types.h"
#include <vector>
#include <string>
#include <memory>

namespace vde {

enum class RaftRole {
    Follower,
    Candidate,
    Leader
};

struct RaftLogEntry {
    uint64_t term;
    uint64_t index;
    OwnedBuffer command;
};

class RaftConsensusNode {
public:
    RaftConsensusNode(uint32_t node_id, std::vector<uint32_t> cluster_nodes);
    ~RaftConsensusNode() = default;

    void handle_election_timeout();
    bool receive_vote_request(uint32_t candidate_id, uint64_t term, uint64_t last_log_term, uint64_t last_log_index);
    bool receive_append_entries(uint32_t leader_id, uint64_t term, uint64_t prev_log_index, uint64_t prev_log_term, const std::vector<RaftLogEntry>& entries);

    Status submit_command(Span<const byte_t> command);

    RaftRole role() const { return role_; }
    uint64_t current_term() const { return current_term_; }
    uint64_t commit_index() const { return commit_index_; }

private:
    uint32_t node_id_;
    std::vector<uint32_t> cluster_nodes_;
    RaftRole role_ = RaftRole::Follower;

    uint64_t current_term_ = 0;
    int32_t voted_for_ = -1;
    std::vector<RaftLogEntry> log_;

    uint64_t commit_index_ = 0;
    uint64_t last_applied_ = 0;
};

} // namespace vde
