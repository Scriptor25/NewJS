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

NJS::ValuePtr NJS::UnaryExpression::GenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    static const std::map<std::string_view, std::pair<bool, UnaryOperator>> fns
    {
        {"++"sv, {true, OperatorInc}},
        {"--"sv, {true, OperatorDec}},
        {"-"sv, {false, OperatorNeg}},
        {"!"sv, {false, OperatorLNot}},
        {"~"sv, {false, OperatorNot}},
        {"&"sv, {false, OperatorRef}},
        {"*"sv, {false, OperatorDeref}},
    };

    auto operand = Operand->GenLLVM(builder, expected_type);
    const auto value = operand->Load(Where);

    if (fns.contains(Operator))
    {
        auto &[assign_, operator_] = fns.at(Operator);
        if (auto result_value = operator_(builder, Where, operand); result_value)
        {
            if (assign_)
            {
                operand->Store(Where, result_value);
                if (Post)
                    return RValue::Create(builder, operand->GetType(), value);
                return operand;
            }
            return result_value;
        }
    }

    Error(Where, "undefined unary operator {}{}", Operator, operand->GetType());
}

std::ostream &NJS::UnaryExpression::Print(std::ostream &stream)
{
    return Operand->Print(stream << (Post ? "" : Operator)) << (Post ? Operator : "");
}
