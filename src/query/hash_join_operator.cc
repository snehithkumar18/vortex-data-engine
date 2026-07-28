#include "vde/query/hash_join_operator.h"

namespace vde {

HashJoinOperator::HashJoinOperator(std::unique_ptr<PhysicalOperator> left_child,
                                   std::unique_ptr<PhysicalOperator> right_child,
                                   uint16_t left_key_idx,
                                   uint16_t right_key_idx)
    : left_child_(std::move(left_child)), right_child_(std::move(right_child)),
      left_key_idx_(left_key_idx), right_key_idx_(right_key_idx) {}

Status HashJoinOperator::open() {
    hashtable_.clear();
    current_match_idx_ = 0;

    if (!left_child_ || !right_child_) return Status::InvalidArgument;
    Status st = left_child_->open();
    if (st != Status::Ok) return st;


    while (true) {
        auto res = left_child_->next();
        if (!res.has_value()) break;
        uint32_t key = 0;
        if (left_key_idx_ < res.value.fields.size()) {
            key = res.value.fields[left_key_idx_].as_u32();
        }
        hashtable_.push_back({key, std::move(res.value)});
    }

    return right_child_->open();
}

Result<Record> HashJoinOperator::next() {
    while (true) {
        auto res = right_child_->next();
        if (!res.has_value()) return res;

        uint32_t right_key = 0;
        if (right_key_idx_ < res.value.fields.size()) {
            right_key = res.value.fields[right_key_idx_].as_u32();
        }

        for (const auto& bucket : hashtable_) {
            if (bucket.key == right_key) {
                Record joined = bucket.record;
                for (const auto& f : res.value.fields) {
                    joined.fields.push_back(f);
                }
                return Result<Record>::ok(std::move(joined));
            }
        }
    }
}

void HashJoinOperator::close() {
    hashtable_.clear();
    if (left_child_) left_child_->close();
    if (right_child_) right_child_->close();
}

}
