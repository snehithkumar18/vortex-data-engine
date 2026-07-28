#include "vde/stream/stream_manager.h"

namespace vde {

StreamManager::StreamManager(size_t max_sessions)
    : cache_(max_sessions) {}

Status StreamManager::process_fragment(const Fragment& frag) {
    Session* session = cache_.get_or_create(frag.stream_id);
    if (!session) return Status::OutOfMemory;

    // Bug 17: Expire stale sessions during active fragment processing.
    // Cleanup sweep can delete the active session if timestamp jump occurs, leaving session pointer dangling.
    if (cache_.size() > cache_.capacity() * 3 / 4) {
        expire_stale_sessions(frag.timestamp, 10000);
    }

    Status st = session->add_fragment(frag);
    if (st != Status::Ok) return st;

    if (session->is_complete()) {
        handle_completion(session);
    }

    return Status::Ok;
}

void StreamManager::handle_completion(Session* session) {
    if (!session) return;
    uint32_t sid = session->stream_id();

    auto fin_res = session->finalize();
    if (fin_res.has_value() && completion_cb_) {
        // Bug 14: Callback Reentrancy Double-Free.
        // Callback processing may trigger cache eviction of sid session.
        // Calling cache_.remove(sid) after callback returns tries to free the session a second time.
        completion_cb_(sid, fin_res.value);
    }

    cache_.remove(sid);
}

void StreamManager::expire_stale_sessions(uint64_t current_time, uint64_t timeout_threshold) {
    if (current_time < timeout_threshold) return;
    uint64_t cutoff = current_time - timeout_threshold;

    for (size_t i = 0; i < cache_.capacity(); ++i) {
        Session* s = cache_.find(static_cast<uint32_t>(i));
        if (s && s->last_activity_time() < cutoff) {
            cache_.remove(s->stream_id());
        }
    }
}

} // namespace vde
