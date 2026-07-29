#pragma once

#include "vde/common/types.h"
#include "vde/transaction/transaction_manager.h"
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace vde {

struct TupleVersion {
    tx_id_t xmin;
    tx_id_t xmax;
    uint32_t version_id;
    OwnedBuffer data;
    TupleVersion* prev = nullptr;
};

class VersionedStreamTracker {
public:
    VersionedStreamTracker();
    ~VersionedStreamTracker();

    Status insert_tuple(tx_id_t tx_id, uint64_t row_id, Span<const byte_t> tuple_data);
    Status update_tuple(tx_id_t tx_id, uint64_t row_id, Span<const byte_t> new_data);
    Status delete_tuple(tx_id_t tx_id, uint64_t row_id);

    Status read_tuple(tx_id_t tx_id, uint64_t row_id, OwnedBuffer* out_data) const;

    void vacuum_garbage_collect(tx_id_t oldest_active_tx);
    size_t total_versions(uint64_t row_id) const;

private:
    bool is_version_visible(tx_id_t tx_id, const TupleVersion& version) const;

    mutable std::shared_mutex engine_mutex_;
    std::vector<TupleVersion*> row_chains_;
    size_t max_rows_ = 65536;
};

}
