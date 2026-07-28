#pragma once

#include "vde/query/sql_ast.h"
#include <memory>
#include <vector>

namespace vde {

class OptimizerRule {
public:
    virtual ~OptimizerRule() = default;
    virtual bool apply(SelectQueryNode* node) = 0;
    virtual const char* name() const = 0;
};

class ConstantFoldingRule : public OptimizerRule {
public:
    bool apply(SelectQueryNode* node) override;
    const char* name() const override { return "ConstantFoldingRule"; }
};

class PredicatePushdownRule : public OptimizerRule {
public:
    bool apply(SelectQueryNode* node) override;
    const char* name() const override { return "PredicatePushdownRule"; }
};

class ProjectionPruningRule : public OptimizerRule {
public:
    bool apply(SelectQueryNode* node) override;
    const char* name() const override { return "ProjectionPruningRule"; }
};

class RuleBasedOptimizer {
public:
    RuleBasedOptimizer();
    void add_rule(std::unique_ptr<OptimizerRule> rule);
    Status optimize(SelectQueryNode* node);

private:
    std::vector<std::unique_ptr<OptimizerRule>> rules_;
};

}
