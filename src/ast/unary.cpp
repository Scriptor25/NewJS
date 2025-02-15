#include <utility>
#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

using namespace std::string_view_literals;

NJS::UnaryExpression::UnaryExpression(
    SourceLocation where,
    std::string_view operator_,
    const bool prefix,
    ExpressionPtr operand)
    : Expression(std::move(where)),
      Operator(std::move(operator_)),
      Prefix(prefix),
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

    if (auto [
            result_type_,
            value_type_,
            callee_
        ] = builder.FindOperator(Operator, Prefix, operand);
        callee_)
    {
        const auto function_type = llvm::FunctionType::get(
            result_type_->GetLLVM(Where, builder),
            {
                value_type_->GetLLVM(Where, builder),
            },
            false);
        const auto result_value = builder.GetBuilder().CreateCall(
            function_type,
            callee_,
            {
                value_type_->IsReference() ? operand->GetPtr(Where) : operand->Load(Where),
            });
        if (result_type_->IsReference())
            return LValue::Create(builder, result_type_->GetElement(), result_value);
        return RValue::Create(builder, result_type_, result_value);
    }

    const auto bkp_value = operand->Load(Where);

    if (fns.contains(Operator))
    {
        auto &[assign_, operator_] = fns.at(Operator);
        if (auto result_value = operator_(builder, Where, operand); result_value)
        {
            if (!assign_)
                return result_value;

            operand->Store(Where, result_value);
            if (Prefix)
                return operand;

            return RValue::Create(builder, operand->GetType(), bkp_value);
        }
    }

    Error(Where, "undefined unary operator {}{}{}", Prefix ? Operator : "", operand->GetType(), Prefix ? "" : Operator);
}

std::ostream &NJS::UnaryExpression::Print(std::ostream &stream)
{
    return Operand->Print(stream << (Prefix ? Operator : "")) << (Prefix ? "" : Operator);
}
