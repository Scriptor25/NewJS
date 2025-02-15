#include <set>
#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

using namespace std::string_view_literals;

NJS::BinaryExpression::BinaryExpression(
    SourceLocation where,
    std::string_view operator_,
    ExpressionPtr left_operand,
    ExpressionPtr right_operand)
    : Expression(std::move(where)),
      Operator(std::move(operator_)),
      LeftOperand(std::move(left_operand)),
      RightOperand(std::move(right_operand))
{
}

NJS::ValuePtr NJS::BinaryExpression::GenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    static const std::map<std::string_view, BinaryOperator> binary_operators
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

    static const std::set compare_operators
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

    const auto is_comparator = compare_operators.contains(Operator);

    auto left_operand = LeftOperand->GenLLVM(builder, !is_comparator ? expected_type : nullptr);
    auto right_operand = RightOperand->GenLLVM(builder, !is_comparator ? expected_type : nullptr);

    if (auto [
            result_type_,
            left_type_,
            right_type_,
            callee_
        ] = builder.FindOperator(
            Operator,
            left_operand,
            right_operand);
        callee_)
    {
        const auto function_type = llvm::FunctionType::get(
            result_type_->GetLLVM(Where, builder),
            {
                left_type_->GetLLVM(Where, builder),
                right_type_->GetLLVM(Where, builder),
            },
            false);
        const auto result_value = builder.GetBuilder().CreateCall(
            function_type,
            callee_,
            {
                left_type_->IsReference() ? left_operand->GetPtr(Where) : left_operand->Load(Where),
                right_type_->IsReference() ? right_operand->GetPtr(Where) : right_operand->Load(Where)
            });
        if (result_type_->IsReference())
            return LValue::Create(builder, result_type_->GetElement(), result_value);
        return RValue::Create(builder, result_type_, result_value);
    }

    auto destination = left_operand;

    if (Operator == "=")
    {
        destination->Store(Where, right_operand);
        return destination;
    }

    const auto left_type = left_operand->GetType();
    const auto right_type = right_operand->GetType();
    const auto operand_type = GetHigherOrderOf(builder.GetTypeContext(), left_type, right_type);
    if (!operand_type)
        Error(Where, "cannot determine higher order type of {} and {}", left_type, right_type);

    left_operand = builder.CreateCast(Where, left_operand, operand_type);
    right_operand = builder.CreateCast(Where, right_operand, operand_type);

    auto operator_ = Operator;
    const auto assign = assignment_operators.contains(operator_);
    if (assign)
        operator_.pop_back();

    if (binary_operators.contains(operator_))
        if (auto result_value = binary_operators.at(operator_)(
            builder,
            Where,
            operand_type,
            left_operand->Load(Where),
            right_operand->Load(Where)))
        {
            if (!assign)
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
