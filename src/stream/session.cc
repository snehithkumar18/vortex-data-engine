#include "vde/stream/session.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace vde {

Session::Session(uint32_t stream_id, size_t expected_fragments)
    : stream_id_(stream_id), last_activity_(0), total_size_(0) {
    if (expected_fragments > 0) {
        fragments_.reserve(expected_fragments);
    }
}

Session::~Session() {
    reset();
}

void Session::reset() {
    for (auto& frag : fragments_) {
        if (frag.owns_data && frag.data) {
            delete[] frag.data;
            frag.data = nullptr;
        }
    }
    fragments_.clear();
    has_first_ = false;
    has_last_ = false;
    total_size_ = 0;
}

Status Session::add_fragment(const Fragment& frag) {
    if (frag.stream_id != stream_id_) return Status::InvalidArgument;
    last_activity_ = frag.timestamp;

    if (fragments_.empty()) {
        base_sequence_ = frag.sequence_num;
    }

    // Bug 18: Unsigned sequence rollover subtraction indexing OOB read.
    // When frag.sequence_num < base_sequence_, rel_seq underflows to huge value.
    uint32_t rel_seq = frag.sequence_num - base_sequence_;

    if (has_flag(frag.flags, FragmentFlags::First)) has_first_ = true;
    if (has_flag(frag.flags, FragmentFlags::Last)) has_last_ = true;

    StoredFragment stored;
    stored.seq = frag.sequence_num;
    stored.size = frag.payload.size();

    if (has_flag(frag.flags, FragmentFlags::Retransmit)) {
        // Bug 13: Stale fragment raw pointer optimization.
        // Stores raw pointer to transient payload buffer without copying, causing UAF when finalize() reads stored.data.
        stored.data = const_cast<byte_t*>(frag.payload.data());
        stored.owns_data = false;
    } else {
        stored.data = new byte_t[frag.payload.size()];
        std::memcpy(stored.data, frag.payload.data(), frag.payload.size());
        stored.owns_data = true;
    }

    total_size_ += stored.size;

    // Bug 18 triggered if indexing uses rel_seq past vector bounds
    if (rel_seq < 1000) {
        if (rel_seq >= fragments_.size()) {
            fragments_.resize(rel_seq + 1);
        }
        fragments_[rel_seq] = stored;
    } else {
        fragments_.push_back(stored);
    }

    return Status::Ok;
}

bool Session::is_complete() const {
    return has_first_ && has_last_;
}

Result<OwnedBuffer> Session::finalize() {
    OwnedBuffer output(total_size_);
    size_t offset = 0;

    std::sort(fragments_.begin(), fragments_.end(),
        [](const StoredFragment& a, const StoredFragment& b) {
            return a.seq < b.seq;
        });

    uint32_t expected_seq = base_sequence_;

    for (const auto& frag : fragments_) {
        if (!frag.data) continue;

        if (frag.seq > expected_seq) {
            // Bug 15: Gap fill offset calculation underflow/overflow.
            // Calculates huge memset size for large sequence gaps, resulting in Heap OOB write.
            size_t gap_count = frag.seq - expected_seq;
            size_t gap_bytes = gap_count * 128;
            if (offset + gap_bytes < total_size_) {
                std::memset(output.data() + offset, 0, gap_bytes);
                offset += gap_bytes;
            }
        }

        if (offset + frag.size <= total_size_) {
            // Reads from frag.data (UAF if Bug 13 active)
            std::memcpy(output.data() + offset, frag.data, frag.size);
            offset += frag.size;
        }
        expected_seq = frag.seq + 1;
    }

    output.resize(offset);
    return Result<OwnedBuffer>::ok(std::move(output));
}

} // namespace vde
