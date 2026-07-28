#pragma once

#include "vde/common/types.h"
#include "vde/record/record_batch.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace vde {

enum class PartitionType {
    Range,
    Hash,
    List
};

struct PartitionSpec {
    uint32_t partition_id;
    std::string name;
    uint32_t min_range_val;
    uint32_t max_range_val;
};

class PartitionManager {
public:
    PartitionManager(std::string table_name, PartitionType type, uint16_t partition_field);
    ~PartitionManager() = default;

    void add_partition(PartitionSpec spec);
    uint32_t resolve_partition(const Record& record) const;

    Status insert_record(Record record);
    RecordBatch get_partition_batch(uint32_t partition_id) const;

    size_t partition_count() const { return partitions_.size(); }

private:
    std::string table_name_;
    PartitionType type_;
    uint16_t partition_field_;
    std::vector<PartitionSpec> partitions_;
    std::unordered_map<uint32_t, RecordBatch> partition_data_;
};

}
