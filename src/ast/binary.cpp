#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Value.hpp>

NJS::BinaryExpr::BinaryExpr(SourceLocation where, TypePtr type, std::string op, ExprPtr lhs, ExprPtr rhs)
    : Expr(std::move(where), std::move(type)), Op(std::move(op)), Lhs(std::move(lhs)), Rhs(std::move(rhs))
{
}

NJS::ValuePtr NJS::BinaryExpr::GenLLVM(Builder& builder)
{
    static const std::map<std::string, std::function<ValuePtr(Builder&, const ValuePtr&, const ValuePtr&)>> fns
    {
        {"==", {OperatorEQ}},
        {"!=", {OperatorNE}},
        {"<", {OperatorLT}},
        {"<=", OperatorLE},
        {">", {OperatorGT}},
        {">=", {OperatorGE}},
        {"||", {OperatorLOr}},
        {"^^", {OperatorLXor}},
        {"&&", {OperatorLAnd}},
        {"|", {OperatorOr}},
        {"^", {OperatorXor}},
        {"&", {OperatorAnd}},
        {"+", OperatorAdd},
        {"-", OperatorSub},
        {"*", {OperatorMul}},
        {"/", {OperatorDiv}},
        {"%", {OperatorRem}},
        {"**", {OperatorPow}},
        {"<<", {OperatorShL}},
        {">>", {OperatorShR}},
    };

    auto lhs = Lhs->GenLLVM(builder);
    const auto rhs = Rhs->GenLLVM(builder);

    auto op = Op;
    if (op == "=")
    {
        lhs->Store(rhs);
        return lhs;
    }

    if (const auto& fn = fns.at(op))
        if (auto value = fn(builder, lhs, rhs))
            return value;

    const auto assign = op.back() == '=';
    if (assign) op.pop_back();

    if (const auto& fn = fns.at(op))
        if (auto value = fn(builder, lhs, rhs))
        {
            if (assign)
            {
                lhs->Store(value);
                return lhs;
            }
            return value;
        }

    Error(Where, "undefined binary operator '{} {} {}'", lhs->GetType(), Op, rhs->GetType());
}

std::ostream& NJS::BinaryExpr::Print(std::ostream& os)
{
    return Rhs->Print(Lhs->Print(os) << ' ' << Op << ' ');
}
