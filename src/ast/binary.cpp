#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Value.hpp>

#include "NJS/Type.hpp"

NJS::BinaryExpr::BinaryExpr(SourceLocation where, TypePtr type, std::string op, ExprPtr lhs, ExprPtr rhs)
    : Expr(std::move(where), std::move(type)), Op(std::move(op)), Lhs(std::move(lhs)), Rhs(std::move(rhs))
{
}

NJS::ValuePtr NJS::BinaryExpr::GenLLVM(Builder& builder)
{
    static const std::map<std::string, std::function<ValuePtr(Builder&, const TypePtr&, llvm::Value*, llvm::Value*)>>
        fns
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

    if (auto [result_, callee_] = builder.GetOp(Op, lhs->GetType(), rhs->GetType());
        result_ && callee_)
    {
        const auto fn_type = llvm::FunctionType::get(
            result_->GetLLVM(builder),
            {lhs->GetType()->GetLLVM(builder), rhs->GetType()->GetLLVM(builder),},
            false);
        const auto value = builder.GetBuilder().CreateCall(fn_type, callee_, {lhs->Load(), rhs->Load()});
        return RValue::Create(builder, result_, value);
    }

    if (lhs->GetType() != rhs->GetType())
        Error("invalid binary operation: type mismatch, {} != {}", lhs->GetType(), rhs->GetType());

    auto op = Op;
    if (op == "=")
    {
        lhs->Store(rhs);
        return lhs;
    }

    if (fns.contains(op))
        if (auto value = fns.at(op)(builder, lhs->GetType(), lhs->Load(), rhs->Load()))
            return value;

    const auto assign = op.back() == '=';
    if (assign) op.pop_back();

    if (fns.contains(op))
        if (auto value = fns.at(op)(builder, lhs->GetType(), lhs->Load(), rhs->Load()))
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
