#pragma once

#include "vde/query/sql_parser.h"
#include "vde/record/record_batch.h"
#include "vde/query/evaluator.h"
#include <memory>
#include <vector>

namespace vde {

class PhysicalOperator {
public:
    virtual ~PhysicalOperator() = default;
    virtual Status open() = 0;
    virtual Result<Record> next() = 0;
    virtual void close() = 0;
};

class SeqScanOperator : public PhysicalOperator {
public:
    SeqScanOperator(const RecordBatch* batch);
    Status open() override;
    Result<Record> next() override;
    void close() override;

private:
    const RecordBatch* batch_;
    size_t current_idx_ = 0;
};

class FilterOperator : public PhysicalOperator {
public:
    FilterOperator(std::unique_ptr<PhysicalOperator> child, const ExprNode* expr);
    Status open() override;
    Result<Record> next() override;
    void close() override;

private:
    std::unique_ptr<PhysicalOperator> child_;
    const ExprNode* expr_;
    QueryEvaluator evaluator_;
};

class LimitOperator : public PhysicalOperator {
public:
    LimitOperator(std::unique_ptr<PhysicalOperator> child, size_t limit);
    Status open() override;
    Result<Record> next() override;
    void close() override;

private:
    std::unique_ptr<PhysicalOperator> child_;
    size_t limit_;
    size_t count_ = 0;
};

class QueryPlanner {
public:
    QueryPlanner() = default;
    std::unique_ptr<PhysicalOperator> create_plan(const SelectStatement& stmt, const RecordBatch* batch);
};

}
