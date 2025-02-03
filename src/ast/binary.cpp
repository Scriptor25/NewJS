#include <set>
#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

using namespace std::string_view_literals;

NJS::BinaryExpr::BinaryExpr(SourceLocation where, std::string operator_, ExprPtr left_operand, ExprPtr right_operand)
    : Expr(std::move(where)),
      Operator(std::move(operator_)),
      LeftOperand(std::move(left_operand)),
      RightOperand(std::move(right_operand))
{
}

NJS::ValuePtr NJS::BinaryExpr::GenLLVM(Builder& builder, const TypePtr& expected_type) const
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

    auto destination = left_operand;

    const auto left_type = left_operand->GetType();
    const auto right_type = right_operand->GetType();

    if (assignment_operators.contains(Operator))
    {
        const auto left_type_reference = builder.GetCtx().GetRefType(left_type);
        if (auto [result_, callee_] = builder.GetOp(Operator, left_type_reference, right_type); result_ && callee_)
        {
            const auto function_type = llvm::FunctionType::get(
                result_->GetLLVM(Where, builder),
                {
                    left_type_reference->GetLLVM(Where, builder),
                    right_type->GetLLVM(Where, builder),
                },
                false);
            const auto result_pointer = builder.GetBuilder().CreateCall(
                function_type,
                callee_,
                {
                    left_operand->GetPtr(Where),
                    right_operand->Load(Where)
                });
            return LValue::Create(builder, result_->GetElement(), result_pointer);
        }
    }
    else
    {
        if (auto [result_, callee_] = builder.GetOp(Operator, left_type, right_type); result_ && callee_)
        {
            const auto function_type = llvm::FunctionType::get(
                result_->GetLLVM(Where, builder),
                {
                    left_type->GetLLVM(Where, builder),
                    right_type->GetLLVM(Where, builder),
                },
                false);
            const auto result_value = builder.GetBuilder().CreateCall(
                function_type,
                callee_,
                {
                    left_operand->Load(Where),
                    right_operand->Load(Where),
                });
            return RValue::Create(builder, result_, result_value);
        }
    }

    auto operator_ = Operator;
    if (operator_ == "=")
    {
        destination->Store(Where, right_operand);
        return destination;
    }

    const auto result_type = max(builder.GetCtx(), left_type, right_type);
    left_operand = builder.CreateCast(Where, left_operand, result_type);
    right_operand = builder.CreateCast(Where, right_operand, result_type);

    if (binary_operators.contains(operator_))
        if (auto result_value = binary_operators.at(operator_)(
            builder,
            Where,
            result_type,
            left_operand->Load(Where),
            right_operand->Load(Where)))
            return result_value;

    const auto assign = operator_.back() == '=';
    if (assign) operator_.pop_back();

    if (binary_operators.contains(operator_))
        if (auto result_value = binary_operators.at(operator_)(
            builder,
            Where,
            result_type,
            left_operand->Load(Where),
            right_operand->Load(Where)))
        {
            if (assign)
            {
                destination->Store(Where, result_value);
                return destination;
            }
            return result_value;
        }

    Error(Where, "undefined binary operator '{} {} {}'", result_type, Operator, result_type);
}

std::ostream& NJS::BinaryExpr::Print(std::ostream& os)
{
    return RightOperand->Print(LeftOperand->Print(os) << ' ' << Operator << ' ');
}
