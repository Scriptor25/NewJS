#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::BinaryExpr::BinaryExpr(std::string op, ExprPtr lhs, ExprPtr rhs)
    : Op(std::move(op)), Lhs(std::move(lhs)), Rhs(std::move(rhs))
{
}

NJS::ValuePtr NJS::BinaryExpr::GenLLVM(Builder& builder)
{
    static std::map<std::string, std::function<ValuePtr(Builder&, const ValuePtr&, const ValuePtr&)>> ops
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
        {"//", {OperatorRoot}},
        {"<<", {OperatorShL}},
        {">>", {OperatorShR}},
    };

    auto lhs = Lhs->GenLLVM(builder);
    const auto rhs = Rhs->GenLLVM(builder);

    if (Op == "=")
    {
        lhs->Store(rhs->Load());
        return lhs;
    }

    if (const auto& op = ops[Op]; op)
        if (auto value = op(builder, lhs, rhs))
            return value;

    const auto assign = Op.back() == '=';
    const auto o = assign ? Op.substr(0, Op.size() - 1) : Op;

    if (const auto& op = ops[o]; op)
        if (auto value = op(builder, lhs, rhs))
        {
            if (assign)
            {
                lhs->Store(value->Load());
                return lhs;
            }
            return value;
        }

    Error("undefined binary operator '{} {} {}'", lhs->GetType(), Op, rhs->GetType());
}

std::ostream& NJS::BinaryExpr::Print(std::ostream& os)
{
    return Rhs->Print(Lhs->Print(os) << ' ' << Op << ' ');
}
