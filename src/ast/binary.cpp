#include <set>
#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::BinaryExpr::BinaryExpr(SourceLocation where, std::string op, ExprPtr lhs, ExprPtr rhs)
    : Expr(std::move(where)), Op(std::move(op)), Lhs(std::move(lhs)), Rhs(std::move(rhs))
{
}

NJS::ValuePtr NJS::BinaryExpr::GenLLVM(Builder& builder)
{
    static const std::map<std::string, BinOp>
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

    static const std::set<std::string> is_assign
    {
        "||=",
        "^^=",
        "&&=",
        "|=",
        "^=",
        "&=",
        "+=",
        "-=",
        "*=",
        "/=",
        "%=",
        "**=",
        "<<=",
        ">>=",
    };

    auto lhs = Lhs->GenLLVM(builder);
    const auto rhs = Rhs->GenLLVM(builder);

    const auto lty = lhs->GetType();
    const auto rty = rhs->GetType();

    if (is_assign.contains(Op))
    {
        const auto ref_lty = builder.GetCtx().GetRefType(lty);
        if (auto [result_, callee_] = builder.GetOp(Op, ref_lty, rty); result_ && callee_)
        {
            const auto fn_type = llvm::FunctionType::get(
                result_->GetLLVM(builder),
                {ref_lty->GetLLVM(builder), rty->GetLLVM(builder),},
                false);
            const auto ptr = builder.GetBuilder().CreateCall(fn_type, callee_, {lhs->GetPtr(), rhs->Load()});
            return LValue::Create(builder, result_->GetElement(), ptr);
        }
    }
    else
    {
        if (auto [result_, callee_] = builder.GetOp(Op, lty, rty); result_ && callee_)
        {
            const auto fn_type = llvm::FunctionType::get(
                result_->GetLLVM(builder),
                {lty->GetLLVM(builder), rty->GetLLVM(builder),},
                false);
            const auto value = builder.GetBuilder().CreateCall(fn_type, callee_, {lhs->Load(), rhs->Load()});
            return RValue::Create(builder, result_, value);
        }
    }

    if (lty != rty)
        Error(Where, "undefined binary operator '{} {} {}'", lty, Op, rty);

    auto op = Op;
    if (op == "=")
    {
        lhs->Store(rhs);
        return lhs;
    }

    if (fns.contains(op))
        if (auto value = fns.at(op)(builder, lty, lhs->Load(), rhs->Load()))
            return value;

    const auto assign = op.back() == '=';
    if (assign) op.pop_back();

    if (fns.contains(op))
        if (auto value = fns.at(op)(builder, lty, lhs->Load(), rhs->Load()))
        {
            if (assign)
            {
                lhs->Store(value);
                return lhs;
            }
            return value;
        }

    Error(Where, "undefined binary operator '{} {} {}'", lty, Op, rty);
}

std::ostream& NJS::BinaryExpr::Print(std::ostream& os)
{
    return Rhs->Print(Lhs->Print(os) << ' ' << Op << ' ');
}
