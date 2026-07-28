#include "vde/query/optimizer_rules.h"

namespace vde {

RuleBasedOptimizer::RuleBasedOptimizer() {
    rules_.push_back(std::make_unique<ConstantFoldingRule>());
    rules_.push_back(std::make_unique<PredicatePushdownRule>());
    rules_.push_back(std::make_unique<ProjectionPruningRule>());
}

void RuleBasedOptimizer::add_rule(std::unique_ptr<OptimizerRule> rule) {
    if (rule) rules_.push_back(std::move(rule));
}

Status RuleBasedOptimizer::optimize(SelectQueryNode* node) {
    if (!node) return Status::InvalidArgument;
    for (const auto& rule : rules_) {
        rule->apply(node);
    }
    return Status::Ok;
}

bool ConstantFoldingRule::apply(SelectQueryNode* node) {
    (void)node;
    return true;
}

bool PredicatePushdownRule::apply(SelectQueryNode* node) {
    (void)node;
    return true;
}

bool ProjectionPruningRule::apply(SelectQueryNode* node) {
    (void)node;
    return true;
}

} // namespace vde
