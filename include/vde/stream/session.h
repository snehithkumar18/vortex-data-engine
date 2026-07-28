#pragma once

#include "vde/stream/fragment.h"
#include <vector>

namespace vde {

class Session {
public:
    explicit Session(uint32_t stream_id, size_t expected_fragments = 0);
    ~Session();

    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    Status add_fragment(const Fragment& frag);
    bool is_complete() const;
    Result<OwnedBuffer> finalize();

    uint32_t stream_id() const { return stream_id_; }
    size_t fragment_count() const { return fragments_.size(); }
    uint64_t last_activity_time() const { return last_activity_; }
    void reset();

private:
    struct StoredFragment {
        uint32_t seq = 0;
        byte_t* data = nullptr;
        size_t size = 0;
        bool owns_data = false;
    };

    uint32_t stream_id_;
    std::vector<StoredFragment> fragments_;
    bool has_first_ = false;
    bool has_last_ = false;
    uint32_t base_sequence_ = 0;
    uint64_t last_activity_ = 0;
    size_t total_size_ = 0;
};

}
