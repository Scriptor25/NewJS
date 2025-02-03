#include <utility>
#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Value.hpp>

using namespace std::string_view_literals;

NJS::UnaryExpr::UnaryExpr(SourceLocation where, std::string op, const bool op_right, ExprPtr operand)
    : Expr(std::move(where)), Op(std::move(op)), OpRight(op_right), Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::UnaryExpr::GenLLVM(Builder& builder, const TypePtr& expected) const
{
    static const std::map<std::string_view, UnOp> fns
    {
        {"++"sv, OperatorInc},
        {"--"sv, OperatorDec},
        {"-"sv, OperatorNeg},
        {"!"sv, OperatorLNot},
        {"~"sv, OperatorNot},
        {"&"sv, OperatorRef},
        {"*"sv, OperatorDeref},
    };

    auto operand = Operand->GenLLVM(builder, expected);
    const auto value = operand->Load(Where);

    if (fns.contains(Op))
        if (auto [value_, assign_] = fns.at(Op)(builder, Where, operand); value_)
        {
            if (assign_)
            {
                operand->Store(Where, value_);
                if (OpRight)
                    return RValue::Create(builder, operand->GetType(), value);
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
