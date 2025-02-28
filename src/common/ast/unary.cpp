#include <utility>
#include <llvm/IR/Value.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
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

NJS::ValuePtr NJS::UnaryExpression::PGenLLVM(Builder &builder, const TypePtr &expected_type) const
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
        const auto result_type = result_.GetLLVM(builder);
        const auto value_type = value_.GetLLVM(builder);

        const auto function_type = llvm::FunctionType::get(result_type, {value_type}, false);

        if (value_.IsReference && !operand->IsLValue())
        {
            const auto value = builder.CreateAlloca(operand->GetType(), true);
            value->StoreNoError(operand);
            operand = value;
        }

        const auto value_arg = value_.IsReference
                                   ? operand->GetPointer()
                                   : operand->Load();

        const auto result_value = builder.GetBuilder().CreateCall(function_type, callee_, {value_arg});
        if (result_.IsReference)
            return LValue::Create(builder, result_.Type, result_value, result_.IsConst);
        return RValue::Create(builder, result_.Type, result_value);
    }

    const auto assign = assignment_operators.contains(Operator);

    if (operators.contains(Operator))
        if (auto result_value = operators.at(Operator)(builder, operand))
        {
            if (!assign)
                return result_value;

            const auto bkp_value = Prefix
                                       ? nullptr
                                       : operand->Load();

            operand->Store(result_value);
            if (Prefix)
                return operand;

            return RValue::Create(builder, operand->GetType(), bkp_value);
        }

    Error(Where, "TODO");
}

std::ostream &NJS::UnaryExpression::Print(std::ostream &stream)
{
    return Operand->Print(stream << (Prefix ? Operator : "")) << (Prefix ? "" : Operator);
}
