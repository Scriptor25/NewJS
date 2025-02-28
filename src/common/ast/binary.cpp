#include <set>
#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/operator.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

using namespace std::string_view_literals;

NJS::BinaryExpression::BinaryExpression(
    SourceLocation where,
    std::string operator_,
    ExpressionPtr left_operand,
    ExpressionPtr right_operand)
    : Expression(std::move(where)),
      Operator(std::move(operator_)),
      LeftOperand(std::move(left_operand)),
      RightOperand(std::move(right_operand))
{
}

NJS::ValuePtr NJS::BinaryExpression::PGenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    static const std::map<std::string_view, BinaryOperator> operators
    {
        {"=="sv, {OperatorEQ}},
        {"!="sv, {OperatorNE}},
        {"<"sv, {OperatorLT}},
        {"<="sv, OperatorLE},
        {">"sv, {OperatorGT}},
        {">="sv, {OperatorGE}},
        {"||"sv, {OperatorLOr}},
        {"^^"sv, {OperatorLXor}},
        {"&&"sv, {OperatorLAnd}},
        {"|"sv, {OperatorOr}},
        {"^"sv, {OperatorXor}},
        {"&"sv, {OperatorAnd}},
        {"+"sv, OperatorAdd},
        {"-"sv, OperatorSub},
        {"*"sv, {OperatorMul}},
        {"/"sv, {OperatorDiv}},
        {"%"sv, {OperatorRem}},
        {"**"sv, {OperatorPow}},
        {"<<"sv, {OperatorShL}},
        {">>"sv, {OperatorShR}},
    };

    static const std::set comparator_operators
    {
        "=="sv,
        "!="sv,
        "<"sv,
        "<="sv,
        ">"sv,
        ">="sv,
    };

    static const std::set assignment_operators
    {
        "="sv,
        "||="sv,
        "^^="sv,
        "&&="sv,
        "|="sv,
        "^="sv,
        "&="sv,
        "+="sv,
        "-="sv,
        "*="sv,
        "/="sv,
        "%="sv,
        "**="sv,
        "<<="sv,
        ">>="sv,
    };

    const auto is_comparator = comparator_operators.contains(Operator);
    const auto is_assignment = assignment_operators.contains(Operator);

    auto left_operand = LeftOperand->GenLLVM(
        builder,
        is_comparator
            ? nullptr
            : expected_type);
    auto right_operand = RightOperand->GenLLVM(
        builder,
        is_comparator
            ? nullptr
            : is_assignment
                  ? left_operand->GetType()
                  : expected_type);

    if (auto [
            result_,
            left_,
            right_,
            callee_
        ] = builder.FindOperator(
            Operator,
            left_operand,
            right_operand);
        callee_)
    {
        const auto function_type = llvm::FunctionType::get(
            result_.GetLLVM(builder),
            {left_.GetLLVM(builder), right_.GetLLVM(builder)},
            false);

        if (left_.IsReference && !left_operand->IsLValue())
        {
            const auto value = builder.CreateAlloca(left_operand->GetType(), true);
            value->StoreNoError(left_operand);
            left_operand = value;
        }

        if (right_.IsReference && !right_operand->IsLValue())
        {
            const auto value = builder.CreateAlloca(right_operand->GetType(), true);
            value->StoreNoError(right_operand);
            right_operand = value;
        }

        const auto result_value = builder.GetBuilder().CreateCall(
            function_type,
            callee_,
            {
                left_.IsReference
                    ? left_operand->GetPointer()
                    : left_operand->Load(),
                right_.IsReference
                    ? right_operand->GetPointer()
                    : right_operand->Load()
            });
        if (result_.IsReference)
            return LValue::Create(builder, result_.Type, result_value, result_.IsConst);
        return RValue::Create(builder, result_.Type, result_value);
    }

    auto destination = left_operand;

    if (Operator == "=")
    {
        destination->Store(right_operand);
        return destination;
    }

    const auto left_type = left_operand->GetType();
    const auto right_type = right_operand->GetType();
    const auto operand_type = GetHigherOrderOf(builder.GetTypeContext(), left_type, right_type);

    left_operand = builder.CreateCast(left_operand, operand_type);
    right_operand = builder.CreateCast(right_operand, operand_type);

    auto operator_ = Operator;
    if (is_assignment)
        operator_.pop_back();

    if (operators.contains(operator_))
        if (auto result_value = operators.at(operator_)(
            builder,
            operand_type,
            left_operand->Load(),
            right_operand->Load()))
        {
            if (!is_assignment)
                return result_value;

            destination->Store(result_value);
            return destination;
        }

    Error(Where, "undefined binary operator {} {} {}", left_type, Operator, right_type);
}

std::ostream &NJS::BinaryExpression::Print(std::ostream &stream)
{
    return RightOperand->Print(LeftOperand->Print(stream) << ' ' << Operator << ' ');
}
