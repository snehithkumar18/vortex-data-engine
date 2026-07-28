#pragma once

#include "vde/common/types.h"
#include "vde/common/skiplist.h"
#include <vector>
#include <string>
#include <memory>

namespace vde {

struct SstableMeta {
    uint32_t level;
    uint32_t sstable_id;
    uint32_t min_key;
    uint32_t max_key;
    size_t file_size;
    std::string file_path;
};

class LsmTreeEngine {
public:
    explicit LsmTreeEngine(std::string db_path, size_t memtable_capacity = 1000);
    ~LsmTreeEngine() = default;

    Status put(uint32_t key, uint32_t value);
    bool get(uint32_t key, uint32_t* out_value) const;
    Status delete_key(uint32_t key);

    void flush_memtable();
    void trigger_compaction(uint32_t target_level);

    size_t memtable_size() const { return memtable_.size(); }
    size_t sstable_count(uint32_t level) const;

private:
    std::string db_path_;
    size_t memtable_capacity_;
    mutable SkipList memtable_;
    std::vector<SstableMeta> sstables_l0_;
    std::vector<SstableMeta> sstables_l1_;
    uint32_t next_sstable_id_ = 1;
};

} // namespace vde
