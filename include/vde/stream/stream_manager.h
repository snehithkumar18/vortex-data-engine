#pragma once

#include "vde/stream/reassembly_cache.h"
#include <functional>

namespace vde {

class StreamManager {
public:
    explicit StreamManager(size_t max_sessions = 256);
    ~StreamManager() = default;

    Status process_fragment(const Fragment& frag);
    void expire_stale_sessions(uint64_t current_time, uint64_t timeout_threshold);

    using CompletionCallback = std::function<void(uint32_t stream_id, OwnedBuffer data)>;
    void set_completion_callback(CompletionCallback cb) { completion_cb_ = std::move(cb); }

    size_t active_session_count() const { return cache_.size(); }

private:
    void handle_completion(Session* session);

    ReassemblyCache cache_;
    CompletionCallback completion_cb_;
};

} // namespace vde
