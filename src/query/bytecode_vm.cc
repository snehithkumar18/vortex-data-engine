#include "vde/query/bytecode_vm.h"

namespace vde {

void BytecodeCompiler::compile_node(const ExprNode* node, std::vector<Instruction>& code) {
    if (!node) return;

    if (node->op() == ExprOp::Literal) {
        Instruction inst;
        inst.opcode = OpCode::OpPushConstU32;
        inst.arg_u32 = node->literal_value().as_u32();
        code.push_back(inst);
    } else if (node->op() == ExprOp::FieldRef) {
        Instruction inst;
        inst.opcode = OpCode::OpLoadField;
        inst.arg_u32 = node->field_id();
        code.push_back(inst);
    } else if (node->op() == ExprOp::Eq || node->op() == ExprOp::Ne ||
               node->op() == ExprOp::And || node->op() == ExprOp::Or) {
        compile_node(node->left(), code);
        compile_node(node->right(), code);

        Instruction inst;
        if (node->op() == ExprOp::Eq) inst.opcode = OpCode::OpEq;
        else if (node->op() == ExprOp::Ne) inst.opcode = OpCode::OpNe;
        else if (node->op() == ExprOp::And) inst.opcode = OpCode::OpAnd;
        else if (node->op() == ExprOp::Or) inst.opcode = OpCode::OpOr;
        code.push_back(inst);
    }
}

std::vector<Instruction> BytecodeCompiler::compile(const ExprNode* root) {
    std::vector<Instruction> code;
    compile_node(root, code);
    Instruction halt;
    halt.opcode = OpCode::OpHalt;
    code.push_back(halt);
    return code;
}

bool BytecodeVM::execute(const std::vector<Instruction>& code, const Record& record) {
    stack_.clear();
    for (const auto& inst : code) {
        switch (inst.opcode) {
            case OpCode::OpPushConstU32:
                stack_.emplace_back(inst.arg_u32);
                break;
            case OpCode::OpLoadField:
                if (inst.arg_u32 < record.fields.size()) {
                    stack_.push_back(record.fields[inst.arg_u32]);
                } else {
                    stack_.emplace_back(uint32_t(0));
                }
                break;
            case OpCode::OpEq: {
                if (stack_.size() < 2) return false;
                FieldValue b = stack_.back(); stack_.pop_back();
                FieldValue a = stack_.back(); stack_.pop_back();
                stack_.emplace_back(uint32_t(a.as_u32() == b.as_u32() ? 1 : 0));
                break;
            }
            case OpCode::OpNe: {
                if (stack_.size() < 2) return false;
                FieldValue b = stack_.back(); stack_.pop_back();
                FieldValue a = stack_.back(); stack_.pop_back();
                stack_.emplace_back(uint32_t(a.as_u32() != b.as_u32() ? 1 : 0));
                break;
            }
            case OpCode::OpAnd: {
                if (stack_.size() < 2) return false;
                FieldValue b = stack_.back(); stack_.pop_back();
                FieldValue a = stack_.back(); stack_.pop_back();
                stack_.emplace_back(uint32_t((a.as_u32() != 0 && b.as_u32() != 0) ? 1 : 0));
                break;
            }
            case OpCode::OpOr: {
                if (stack_.size() < 2) return false;
                FieldValue b = stack_.back(); stack_.pop_back();
                FieldValue a = stack_.back(); stack_.pop_back();
                stack_.emplace_back(uint32_t((a.as_u32() != 0 || b.as_u32() != 0) ? 1 : 0));
                break;
            }
            case OpCode::OpHalt:
                if (!stack_.empty()) {
                    return stack_.back().as_u32() != 0;
                }
                return false;
            default:
                break;
        }
    }
    return false;
}

}
