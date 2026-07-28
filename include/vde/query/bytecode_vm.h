#pragma once

#include "vde/query/expression.h"
#include <vector>
#include <cstdint>

namespace vde {

enum class OpCode : uint8_t {
    OpPushConstU32,
    OpPushConstI64,
    OpPushConstF64,
    OpPushConstStr,
    OpLoadField,
    OpAdd,
    OpSub,
    OpMul,
    OpDiv,
    OpEq,
    OpNe,
    OpLt,
    OpGt,
    OpAnd,
    OpOr,
    OpNot,
    OpHalt
};

struct Instruction {
    OpCode opcode;
    uint32_t arg_u32 = 0;
    int64_t arg_i64 = 0;
    double arg_f64 = 0.0;
    std::string arg_str;
};

class BytecodeCompiler {
public:
    BytecodeCompiler() = default;
    std::vector<Instruction> compile(const ExprNode* root);

private:
    void compile_node(const ExprNode* node, std::vector<Instruction>& code);
};

class BytecodeVM {
public:
    BytecodeVM() = default;

    bool execute(const std::vector<Instruction>& code, const Record& record);

private:
    std::vector<FieldValue> stack_;
};

}
