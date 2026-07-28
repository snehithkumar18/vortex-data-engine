#include "vde/query/sort_merge_join.h"
#include <algorithm>

namespace vde {

SortMergeJoinOperator::SortMergeJoinOperator(std::unique_ptr<PhysicalOperator> left_child,
                                               std::unique_ptr<PhysicalOperator> right_child,
                                               uint16_t left_key_idx,
                                               uint16_t right_key_idx)
    : left_child_(std::move(left_child)), right_child_(std::move(right_child)),
      left_key_idx_(left_key_idx), right_key_idx_(right_key_idx) {}

Status SortMergeJoinOperator::open() {
    sorted_left_.clear();
    sorted_right_.clear();
    left_pos_ = 0;
    right_pos_ = 0;

    if (!left_child_ || !right_child_) return Status::InvalidArgument;
    Status st1 = left_child_->open();
    if (st1 != Status::Ok) return st1;

    while (true) {
        auto res = left_child_->next();
        if (!res.has_value()) break;
        sorted_left_.push_back(std::move(res.value));
    }

    Status st2 = right_child_->open();
    if (st2 != Status::Ok) return st2;

    while (true) {
        auto res = right_child_->next();
        if (!res.has_value()) break;
        sorted_right_.push_back(std::move(res.value));
    }


    uint16_t lidx = left_key_idx_;
    std::sort(sorted_left_.begin(), sorted_left_.end(),
        [lidx](const Record& a, const Record& b) {
            uint32_t ka = (lidx < a.fields.size()) ? a.fields[lidx].as_u32() : 0;
            uint32_t kb = (lidx < b.fields.size()) ? b.fields[lidx].as_u32() : 0;
            return ka < kb;
        });

    uint16_t ridx = right_key_idx_;
    std::sort(sorted_right_.begin(), sorted_right_.end(),
        [ridx](const Record& a, const Record& b) {
            uint32_t ka = (ridx < a.fields.size()) ? a.fields[ridx].as_u32() : 0;
            uint32_t kb = (ridx < b.fields.size()) ? b.fields[ridx].as_u32() : 0;
            return ka < kb;
        });

    return Status::Ok;
}

Result<Record> SortMergeJoinOperator::next() {
    while (left_pos_ < sorted_left_.size() && right_pos_ < sorted_right_.size()) {
        const Record& lrec = sorted_left_[left_pos_];
        const Record& rrec = sorted_right_[right_pos_];

        uint32_t lkey = (left_key_idx_ < lrec.fields.size()) ? lrec.fields[left_key_idx_].as_u32() : 0;
        uint32_t rkey = (right_key_idx_ < rrec.fields.size()) ? rrec.fields[right_key_idx_].as_u32() : 0;

        if (lkey < rkey) {
            left_pos_++;
        } else if (lkey > rkey) {
            right_pos_++;
        } else {
            Record joined = lrec;
            for (const auto& f : rrec.fields) {
                joined.fields.push_back(f);
            }
            right_pos_++;
            return Result<Record>::ok(std::move(joined));
        }
    }
    return Result<Record>::error(Status::Eof);
}

void SortMergeJoinOperator::close() {
    sorted_left_.clear();
    sorted_right_.clear();
    left_pos_ = 0;
    right_pos_ = 0;
    if (left_child_) left_child_->close();
    if (right_child_) right_child_->close();
}

}
