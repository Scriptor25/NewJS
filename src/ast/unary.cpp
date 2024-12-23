#include <utility>
#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Value.hpp>

NJS::UnaryExpr::UnaryExpr(SourceLocation where, std::string op, const bool op_right, ExprPtr operand)
    : Expr(std::move(where)), Op(std::move(op)), OpRight(op_right), Operand(std::move(operand))
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
        {"&", OperatorRef},
    };

    auto operand = Operand->GenLLVM(builder);
    const auto val = operand->Load();

    if (fns.contains(Op))
        if (const auto [value_, assign_] = fns.at(Op)(builder, operand); value_)
        {
            if (assign_)
            {
                operand->Store(value_);
                if (OpRight)
                    return RValue::Create(builder, operand->GetType(), val);
                return operand;
            }
            return value_;
        }

    Error(Where, "undefined unary operator {}{}", Op, operand->GetType());
}

std::ostream& NJS::UnaryExpr::Print(std::ostream& os)
{
    return Operand->Print(os << (OpRight ? "" : Op)) << (OpRight ? Op : "");
}
