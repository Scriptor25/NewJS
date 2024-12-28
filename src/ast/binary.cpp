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

NJS::ValuePtr NJS::BinaryExpr::GenLLVM(Builder& builder, const TypePtr& expected)
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

    static const std::set<std::string> is_cmp
    {
        "==",
        "!=",
        "<",
        "<=",
        ">",
        ">=",
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

    const auto cmp = is_cmp.contains(Op);

    auto lhs = Lhs->GenLLVM(builder, !cmp ? expected : nullptr);
    auto rhs = Rhs->GenLLVM(builder, !cmp ? expected : nullptr);

    auto dst = lhs;

    const auto lty = lhs->GetType();
    const auto rty = rhs->GetType();

    if (is_assign.contains(Op))
    {
        const auto ref_lty = builder.GetCtx().GetRefType(lty);
        if (auto [result_, callee_] = builder.GetOp(Op, ref_lty, rty); result_ && callee_)
        {
            const auto fn_type = llvm::FunctionType::get(
                result_->GetLLVM(Where, builder),
                {ref_lty->GetLLVM(Where, builder), rty->GetLLVM(Where, builder)},
                false);
            const auto ptr = builder.GetBuilder().CreateCall(fn_type, callee_, {lhs->GetPtr(Where), rhs->Load(Where)});
            return LValue::Create(builder, result_->GetElement(), ptr);
        }
    }
    else
    {
        if (auto [result_, callee_] = builder.GetOp(Op, lty, rty); result_ && callee_)
        {
            const auto fn_type = llvm::FunctionType::get(
                result_->GetLLVM(Where, builder),
                {lty->GetLLVM(Where, builder), rty->GetLLVM(Where, builder),},
                false);
            const auto value = builder.GetBuilder().CreateCall(fn_type, callee_, {lhs->Load(Where), rhs->Load(Where)});
            return RValue::Create(builder, result_, value);
        }
    }

    auto op = Op;
    if (op == "=")
    {
        dst->Store(Where, rhs);
        return dst;
    }

    const auto ty = max(builder.GetCtx(), lty, rty);
    lhs = builder.CreateCast(Where, lhs, ty);
    rhs = builder.CreateCast(Where, rhs, ty);

    if (fns.contains(op))
        if (auto value = fns.at(op)(builder, Where, ty, lhs->Load(Where), rhs->Load(Where)))
            return value;

    const auto assign = op.back() == '=';
    if (assign) op.pop_back();

    if (fns.contains(op))
        if (auto value = fns.at(op)(builder, Where, ty, lhs->Load(Where), rhs->Load(Where)))
        {
            if (assign)
            {
                dst->Store(Where, value);
                return dst;
            }
            return value;
        }

    Error(Where, "undefined binary operator '{} {} {}'", ty, Op, ty);
}

std::ostream& NJS::BinaryExpr::Print(std::ostream& os)
{
    return Rhs->Print(Lhs->Print(os) << ' ' << Op << ' ');
}
