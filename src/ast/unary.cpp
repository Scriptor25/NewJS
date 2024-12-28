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

NJS::ValuePtr NJS::UnaryExpr::GenLLVM(Builder& builder, const TypePtr& expected)
{
    static const std::map<std::string, std::function<UnaryResult(Builder&, const SourceLocation&, const ValuePtr&)>> fns
    {
        {"++", OperatorInc},
        {"--", OperatorDec},
        {"-", OperatorNeg},
        {"!", OperatorLNot},
        {"~", OperatorNot},
        {"&", OperatorRef},
    };

    auto operand = Operand->GenLLVM(builder, expected);
    const auto val = operand->Load(Where);

    if (fns.contains(Op))
        if (const auto [value_, assign_] = fns.at(Op)(builder, Where, operand); value_)
        {
            if (assign_)
            {
                operand->Store(Where, value_);
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
