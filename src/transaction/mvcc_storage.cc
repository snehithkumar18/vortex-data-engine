#include "vde/transaction/mvcc_storage.h"

namespace vde {

MvccStorageEngine::MvccStorageEngine() {
    row_chains_.resize(max_rows_, nullptr);
}

MvccStorageEngine::~MvccStorageEngine() {
    std::unique_lock<std::shared_mutex> lock(engine_mutex_);
    for (size_t i = 0; i < row_chains_.size(); ++i) {
        TupleVersion* curr = row_chains_[i];
        while (curr) {
            TupleVersion* prev = curr->prev;
            delete curr;
            curr = prev;
        }
        row_chains_[i] = nullptr;
    }
}

bool MvccStorageEngine::is_version_visible(tx_id_t tx_id, const TupleVersion& version) const {
    if (version.xmin > tx_id) return false; // Created in future transaction
    if (version.xmax != 0 && version.xmax <= tx_id) return false; // Deleted/superseded by past transaction
    return true;
}

Status MvccStorageEngine::insert_tuple(tx_id_t tx_id, uint64_t row_id, Span<const byte_t> tuple_data) {
    if (row_id >= max_rows_) return Status::InvalidArgument;
    std::unique_lock<std::shared_mutex> lock(engine_mutex_);

    TupleVersion* new_ver = new TupleVersion();
    new_ver->xmin = tx_id;
    new_ver->xmax = 0;
    new_ver->version_id = 1;
    new_ver->data.append(tuple_data);
    new_ver->prev = row_chains_[row_id];

    row_chains_[row_id] = new_ver;
    return Status::Ok;
}

Status MvccStorageEngine::update_tuple(tx_id_t tx_id, uint64_t row_id, Span<const byte_t> new_data) {
    if (row_id >= max_rows_) return Status::InvalidArgument;
    std::unique_lock<std::shared_mutex> lock(engine_mutex_);

    TupleVersion* head = row_chains_[row_id];
    if (!head || head->xmax != 0) return Status::NotFound;

    head->xmax = tx_id;

    TupleVersion* new_ver = new TupleVersion();
    new_ver->xmin = tx_id;
    new_ver->xmax = 0;
    new_ver->version_id = head->version_id + 1;
    new_ver->data.append(new_data);
    new_ver->prev = head;

    row_chains_[row_id] = new_ver;
    return Status::Ok;
}

Status MvccStorageEngine::delete_tuple(tx_id_t tx_id, uint64_t row_id) {
    if (row_id >= max_rows_) return Status::InvalidArgument;
    std::unique_lock<std::shared_mutex> lock(engine_mutex_);

    TupleVersion* head = row_chains_[row_id];
    if (!head || head->xmax != 0) return Status::NotFound;

    head->xmax = tx_id;
    return Status::Ok;
}

Status MvccStorageEngine::read_tuple(tx_id_t tx_id, uint64_t row_id, OwnedBuffer* out_data) const {
    if (row_id >= max_rows_ || !out_data) return Status::InvalidArgument;
    std::shared_lock<std::shared_mutex> lock(engine_mutex_);

    const TupleVersion* curr = row_chains_[row_id];
    while (curr) {
        if (is_version_visible(tx_id, *curr)) {
            out_data->clear();
            out_data->append(curr->data.span());
            return Status::Ok;
        }
        curr = curr->prev;
    }
    return Status::NotFound;
}

void MvccStorageEngine::vacuum_garbage_collect(tx_id_t oldest_active_tx) {
    std::unique_lock<std::shared_mutex> lock(engine_mutex_);
    for (size_t i = 0; i < row_chains_.size(); ++i) {
        TupleVersion* curr = row_chains_[i];
        TupleVersion* prev_valid = nullptr;

        while (curr) {
            if (curr->xmax != 0 && curr->xmax < oldest_active_tx) {
                // Prune expired version chain
                TupleVersion* dead = curr;
                if (prev_valid) {
                    prev_valid->prev = curr->prev;
                } else {
                    row_chains_[i] = curr->prev;
                }
                curr = curr->prev;
                delete dead;
            } else {
                prev_valid = curr;
                curr = curr->prev;
            }
        }
    }
}

size_t MvccStorageEngine::total_versions(uint64_t row_id) const {
    if (row_id >= max_rows_) return 0;
    std::shared_lock<std::shared_mutex> lock(engine_mutex_);
    size_t count = 0;
    const TupleVersion* curr = row_chains_[row_id];
    while (curr) {
        count++;
        curr = curr->prev;
    }
    return count;
}

} // namespace vde
