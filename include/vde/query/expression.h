#pragma once

#include "vde/common/types.h"
#include "vde/common/byte_reader.h"
#include "vde/record/field_value.h"

namespace vde {

enum class ExprOp : uint8_t {
    Eq          = 0,
    Ne          = 1,
    Lt          = 2,
    Gt          = 3,
    Le          = 4,
    Ge          = 5,
    And         = 6,
    Or          = 7,
    Not         = 8,
    FieldRef    = 9,
    Literal     = 10,
    Add         = 11,
    Sub         = 12,
    Mul         = 13,
    Div         = 14,
    Mod         = 15,
    Contains    = 16,
    StartsWith  = 17
};

class ExprNode {
public:
    ExprNode() = default;
    ~ExprNode();

    ExprNode(const ExprNode&) = delete;
    ExprNode& operator=(const ExprNode&) = delete;

    static ExprNode* make_literal(FieldValue value);
    static ExprNode* make_field_ref(uint16_t field_id);
    static ExprNode* make_unary(ExprOp op, ExprNode* child);
    static ExprNode* make_binary(ExprOp op, ExprNode* left, ExprNode* right);

    ExprOp op() const { return op_; }
    const FieldValue& literal_value() const { return value_; }
    uint16_t field_id() const { return field_id_; }
    ExprNode* left() const { return left_; }
    ExprNode* right() const { return right_; }

private:
    ExprOp op_ = ExprOp::Literal;
    FieldValue value_{};
    uint16_t field_id_ = 0;
    ExprNode* left_ = nullptr;
    ExprNode* right_ = nullptr;
};

Status parse_expression(ByteReader& reader, ExprNode** out_root);
void free_expression(ExprNode* root);

}
