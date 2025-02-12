#include <utility>
#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Value.hpp>

using namespace std::string_view_literals;

NJS::UnaryExpression::UnaryExpression(
    SourceLocation where,
    std::string_view operator_,
    const bool post,
    ExpressionPtr operand)
    : Expression(std::move(where)),
      Operator(std::move(operator_)),
      Post(post),
      Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::UnaryExpression::GenLLVM(Builder &builder, const TypePtr &expected) const
{
    static const std::map<std::string_view, UnaryOperator> fns
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

    if (fns.contains(Operator))
        if (auto [value_, assign_] = fns.at(Operator)(builder, Where, operand); value_)
        {
            if (assign_)
            {
                operand->Store(Where, value_);
                if (Post)
                    return RValue::Create(builder, operand->GetType(), value);
                return operand;
            }
            return value_;
        }

    Error(Where, "undefined unary operator {}{}", Operator, operand->GetType());
}

std::ostream &NJS::UnaryExpression::Print(std::ostream &os)
{
    return Operand->Print(os << (Post ? "" : Operator)) << (Post ? Operator : "");
}
