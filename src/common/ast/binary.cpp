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

NJS::ValuePtr NJS::BinaryExpression::GenLLVM(
    Builder &builder,
    const TypePtr &expected_type) const
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
            result_.GetLLVM(Where, builder),
            {
                left_.GetLLVM(LeftOperand->Where, builder),
                right_.GetLLVM(RightOperand->Where, builder),
            },
            false);
        if (left_.IsReference && !left_operand->IsLValue())
        {
            const auto value = builder.CreateAlloca(LeftOperand->Where, left_operand->GetType(), true);
            value->StoreForce(LeftOperand->Where, left_operand);
            left_operand = value;
        }
        if (right_.IsReference && !right_operand->IsLValue())
        {
            const auto value = builder.CreateAlloca(RightOperand->Where, right_operand->GetType(), true);
            value->StoreForce(RightOperand->Where, right_operand);
            right_operand = value;
        }
        const auto result_value = builder.GetBuilder().CreateCall(
            function_type,
            callee_,
            {
                left_.IsReference
                    ? left_operand->GetPtr(LeftOperand->Where)
                    : left_operand->Load(LeftOperand->Where),
                right_.IsReference
                    ? right_operand->GetPtr(RightOperand->Where)
                    : right_operand->Load(RightOperand->Where)
            });
        if (result_.IsReference)
            return LValue::Create(builder, result_.Type, result_value, result_.IsConst);
        return RValue::Create(builder, result_.Type, result_value);
    }

    auto destination = left_operand;

    if (Operator == "=")
    {
        destination->Store(Where, right_operand);
        return destination;
    }

    const auto left_type = left_operand->GetType();
    const auto right_type = right_operand->GetType();
    const auto operand_type = GetHigherOrderOf(Where, builder.GetTypeContext(), left_type, right_type);

    left_operand = builder.CreateCast(LeftOperand->Where, left_operand, operand_type);
    right_operand = builder.CreateCast(RightOperand->Where, right_operand, operand_type);

    auto operator_ = Operator;
    if (is_assignment)
        operator_.pop_back();

    if (operators.contains(operator_))
        if (auto result_value = operators.at(operator_)(
            builder,
            Where,
            operand_type,
            left_operand->Load(LeftOperand->Where),
            right_operand->Load(RightOperand->Where)))
        {
            if (!is_assignment)
                return result_value;

            destination->Store(Where, result_value);
            return destination;
        }

    Error(Where, "undefined binary operator '{} {} {}'", operand_type, Operator, operand_type);
}

std::ostream &NJS::BinaryExpression::Print(std::ostream &stream)
{
    return RightOperand->Print(LeftOperand->Print(stream) << ' ' << Operator << ' ');
}
