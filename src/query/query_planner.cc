#include "vde/query/query_planner.h"

namespace vde {

SeqScanOperator::SeqScanOperator(const RecordBatch* batch)
    : batch_(batch), current_idx_(0) {}

Status SeqScanOperator::open() {
    current_idx_ = 0;
    return Status::Ok;
}

Result<Record> SeqScanOperator::next() {
    if (!batch_ || current_idx_ >= batch_->record_count()) {
        return Result<Record>::error(Status::Eof);
    }
    return Result<Record>::ok(batch_->record_at(current_idx_++));
}

void SeqScanOperator::close() {
    current_idx_ = 0;
}

FilterOperator::FilterOperator(std::unique_ptr<PhysicalOperator> child, const ExprNode* expr)
    : child_(std::move(child)), expr_(expr) {}

Status FilterOperator::open() {
    if (!child_) return Status::InvalidArgument;
    return child_->open();
}

Result<Record> FilterOperator::next() {
    while (true) {
        auto res = child_->next();
        if (!res.has_value()) return res;
        if (evaluator_.evaluate(expr_, res.value)) {
            return res;
        }
    }
}

void FilterOperator::close() {
    if (child_) child_->close();
}

LimitOperator::LimitOperator(std::unique_ptr<PhysicalOperator> child, size_t limit)
    : child_(std::move(child)), limit_(limit), count_(0) {}

Status LimitOperator::open() {
    count_ = 0;
    if (!child_) return Status::InvalidArgument;
    return child_->open();
}

Result<Record> LimitOperator::next() {
    if (count_ >= limit_) return Result<Record>::error(Status::Eof);
    auto res = child_->next();
    if (res.has_value()) count_++;
    return res;
}

void LimitOperator::close() {
    count_ = 0;
    if (child_) child_->close();
}

std::unique_ptr<PhysicalOperator> QueryPlanner::create_plan(const SelectStatement& stmt, const RecordBatch* batch) {
    std::unique_ptr<PhysicalOperator> plan = std::make_unique<SeqScanOperator>(batch);
    if (stmt.where_clause) {
        plan = std::make_unique<FilterOperator>(std::move(plan), stmt.where_clause);
    }
    if (stmt.limit > 0) {
        plan = std::make_unique<LimitOperator>(std::move(plan), stmt.limit);
    }
    return plan;
}

}
