#include <utility>
#include <llvm/IR/Value.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/operator.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

using namespace std::string_view_literals;

NJS::UnaryExpression::UnaryExpression(
    SourceLocation where,
    std::string operator_,
    const bool prefix,
    ExpressionPtr operand)
    : Expression(std::move(where)),
      Operator(std::move(operator_)),
      Prefix(prefix),
      Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::UnaryExpression::GenLLVM(
    Builder &builder,
    const TypePtr &expected_type) const
{
    static const std::map<std::string_view, UnaryOperator> operators
    {
        {"++"sv, OperatorInc},
        {"--"sv, OperatorDec},
        {"-"sv, OperatorNeg},
        {"!"sv, OperatorLNot},
        {"~"sv, OperatorNot},
        {"&"sv, OperatorRef},
        {"*"sv, OperatorDeref},
    };

    static const std::set assignment_operators
    {
        "++"sv,
        "--"sv,
    };

    auto operand = Operand->GenLLVM(builder, expected_type);

    if (auto [
            result_,
            value_,
            callee_
        ] = builder.FindOperator(Operator, Prefix, operand);
        callee_)
    {
        const auto function_type = llvm::FunctionType::get(
            result_.GetLLVM(Where, builder),
            {value_.GetLLVM(Operand->Where, builder)},
            false);
        if (value_.IsReference && !operand->IsLValue())
        {
            const auto value = builder.CreateAlloca(Operand->Where, operand->GetType(), true);
            value->StoreForce(Operand->Where, operand);
            operand = value;
        }
        const auto result_value = builder.GetBuilder().CreateCall(
            function_type,
            callee_,
            {
                value_.IsReference
                    ? operand->GetPtr(Operand->Where)
                    : operand->Load(Operand->Where)
            });
        if (result_.IsReference)
            return LValue::Create(builder, result_.Type, result_value, result_.IsConst);
        return RValue::Create(builder, result_.Type, result_value);
    }

    const auto assign = assignment_operators.contains(Operator);

    if (operators.contains(Operator))
        if (auto result_value = operators.at(Operator)(builder, Where, operand))
        {
            if (!assign)
                return result_value;

            const auto bkp_value = Prefix ? nullptr : operand->Load(Operand->Where);

            operand->Store(Where, result_value);
            if (Prefix)
                return operand;

            return RValue::Create(builder, operand->GetType(), bkp_value);
        }

    Error(
        Where,
        "undefined unary operator {}{}{}",
        Prefix ? Operator : std::string(),
        operand->GetType(),
        Prefix ? std::string() : Operator);
}

std::ostream &NJS::UnaryExpression::Print(std::ostream &stream)
{
    return Operand->Print(stream << (Prefix ? Operator : "")) << (Prefix ? "" : Operator);
}
