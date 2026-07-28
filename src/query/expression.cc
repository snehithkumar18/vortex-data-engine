#include "vde/query/expression.h"

namespace vde {

ExprNode::~ExprNode() {
    delete left_;
    delete right_;
}

ExprNode* ExprNode::make_literal(FieldValue value) {
    auto node = new ExprNode();
    node->op_ = ExprOp::Literal;
    node->value_ = std::move(value);
    return node;
}

ExprNode* ExprNode::make_field_ref(uint16_t field_id) {
    auto node = new ExprNode();
    node->op_ = ExprOp::FieldRef;
    node->field_id_ = field_id;
    return node;
}

ExprNode* ExprNode::make_unary(ExprOp op, ExprNode* child) {
    auto node = new ExprNode();
    node->op_ = op;
    node->left_ = child;
    return node;
}

ExprNode* ExprNode::make_binary(ExprOp op, ExprNode* left, ExprNode* right) {
    auto node = new ExprNode();
    node->op_ = op;
    node->left_ = left;
    node->right_ = right;
    return node;
}

Status parse_expression(ByteReader& reader, ExprNode** out_root) {
    if (!out_root) return Status::InvalidArgument;

    auto op_res = reader.read_u8();
    if (!op_res.has_value()) return Status::Truncated;
    ExprOp op = static_cast<ExprOp>(op_res.value);

    if (op == ExprOp::Literal) {
        auto val_res = reader.read_u32_le();
        if (!val_res.has_value()) return Status::Truncated;
        *out_root = ExprNode::make_literal(FieldValue(val_res.value));
        return Status::Ok;
    } else if (op == ExprOp::FieldRef) {
        auto id_res = reader.read_u16_le();
        if (!id_res.has_value()) return Status::Truncated;
        *out_root = ExprNode::make_field_ref(id_res.value);
        return Status::Ok;
    } else if (op == ExprOp::Not) {
        ExprNode* child = nullptr;
        Status st = parse_expression(reader, &child);
        if (st != Status::Ok) return st;
        *out_root = ExprNode::make_unary(op, child);
        return Status::Ok;
    } else {
        ExprNode* left = nullptr;
        ExprNode* right = nullptr;
        Status st1 = parse_expression(reader, &left);
        if (st1 != Status::Ok) return st1;
        Status st2 = parse_expression(reader, &right);
        if (st2 != Status::Ok) {
            delete left;
            return st2;
        }
        *out_root = ExprNode::make_binary(op, left, right);
        return Status::Ok;
    }
}

void free_expression(ExprNode* root) {
    delete root;
}

}
