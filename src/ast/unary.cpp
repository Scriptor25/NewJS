#include <utility>
#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Value.hpp>

NJS::UnaryExpr::UnaryExpr(SourceLocation where, TypePtr type, std::string op, const bool op_right, ExprPtr operand)
    : Expr(std::move(where), std::move(type)), Op(std::move(op)), OpRight(op_right), Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::UnaryExpr::GenLLVM(Builder& builder)
{
    static const std::map<std::string, std::function<std::pair<ValuePtr, bool>(Builder&, const ValuePtr&)>> fns
    {
        {"++", OperatorInc},
        {"--", OperatorDec},
        {"-", OperatorNeg},
        {"!", OperatorLNot},
        {"~", OperatorNot},
    };

    auto operand = Operand->GenLLVM(builder);
    const auto val = operand->Load();

    if (const auto& fn = fns.at(Op))
        if (const auto [value_, assign_] = fn(builder, operand); value_)
        {
            if (assign_)
            {
                operand->Store(value_);
                if (OpRight)
                    return RValue::Create(builder, Type, val);
                return operand;
            }
            return value_;
        }

    Error(Where, "undefined unary operator {}{}", Op, Type);
}

std::ostream& NJS::UnaryExpr::Print(std::ostream& os)
{
    return Operand->Print(os << (OpRight ? "" : Op)) << (OpRight ? Op : "");
}
