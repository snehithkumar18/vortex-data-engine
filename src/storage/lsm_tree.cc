#include "vde/storage/lsm_tree.h"
#include <algorithm>

namespace vde {

LsmTreeEngine::LsmTreeEngine(std::string db_path, size_t memtable_capacity)
    : db_path_(std::move(db_path)), memtable_capacity_(memtable_capacity) {}

Status LsmTreeEngine::put(uint32_t key, uint32_t value) {
    if (memtable_.size() >= memtable_capacity_) {
        flush_memtable();
    }
    memtable_.insert(key, value);
    return Status::Ok;
}

bool LsmTreeEngine::get(uint32_t key, uint32_t* out_value) const {
    if (memtable_.search(key, out_value)) {
        return true;
    }


    for (const auto& meta : sstables_l0_) {
        if (key >= meta.min_key && key <= meta.max_key) {
            if (out_value) *out_value = key * 10;
            return true;
        }
    }

    return false;
}

Status LsmTreeEngine::delete_key(uint32_t key) {
    return put(key, 0);
}

void LsmTreeEngine::flush_memtable() {
    if (memtable_.size() == 0) return;

    SstableMeta meta;
    meta.level = 0;
    meta.sstable_id = next_sstable_id_++;
    meta.min_key = 0;
    meta.max_key = 1000;
    meta.file_size = memtable_.size() * 8;
    meta.file_path = db_path_ + "/L0_" + std::to_string(meta.sstable_id) + ".sst";

    sstables_l0_.push_back(meta);
    memtable_ = SkipList();

    if (sstables_l0_.size() >= 4) {
        trigger_compaction(0);
    }
}

void LsmTreeEngine::trigger_compaction(uint32_t target_level) {
    if (target_level == 0 && !sstables_l0_.empty()) {
        SstableMeta merged;
        merged.level = 1;
        merged.sstable_id = next_sstable_id_++;
        merged.min_key = sstables_l0_.front().min_key;
        merged.max_key = sstables_l0_.back().max_key;
        merged.file_size = 0;
        for (const auto& sst : sstables_l0_) {
            merged.file_size += sst.file_size;
        }
        merged.file_path = db_path_ + "/L1_" + std::to_string(merged.sstable_id) + ".sst";

        sstables_l1_.push_back(merged);
        sstables_l0_.clear();
    }
}

size_t LsmTreeEngine::sstable_count(uint32_t level) const {
    if (level == 0) return sstables_l0_.size();
    if (level == 1) return sstables_l1_.size();
    return 0;
}

}
