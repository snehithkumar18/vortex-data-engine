#include "vde/storage/partition_manager.h"
#include "vde/common/math_utils.h"

namespace vde {

PartitionManager::PartitionManager(std::string table_name, PartitionType type, uint16_t partition_field)
    : table_name_(std::move(table_name)), type_(type), partition_field_(partition_field) {}

void PartitionManager::add_partition(PartitionSpec spec) {
    partitions_.push_back(std::move(spec));
}

uint32_t PartitionManager::resolve_partition(const Record& record) const {
    if (partitions_.empty()) return 0;

    uint32_t val = (partition_field_ < record.fields.size()) ? record.fields[partition_field_].as_u32() : 0;

    if (type_ == PartitionType::Range) {
        for (const auto& p : partitions_) {
            if (val >= p.min_range_val && val <= p.max_range_val) {
                return p.partition_id;
            }
        }
    } else if (type_ == PartitionType::Hash) {
        uint32_t h = mur_hash3_32(reinterpret_cast<const byte_t*>(&val), 4, 0x12345678);
        return partitions_[h % partitions_.size()].partition_id;
    }

    return partitions_[0].partition_id;
}

Status PartitionManager::insert_record(Record record) {
    uint32_t pid = resolve_partition(record);
    partition_data_[pid].add_record(std::move(record));
    return Status::Ok;
}

RecordBatch PartitionManager::get_partition_batch(uint32_t partition_id) const {
    auto it = partition_data_.find(partition_id);
    if (it != partition_data_.end()) {
        return it->second;
    }
    return RecordBatch();
}

} // namespace vde
