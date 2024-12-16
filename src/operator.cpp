#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/TypeContext.hpp>

#include "NJS/Builder.hpp"
#include "NJS/Value.hpp"

NJS::ValuePtr NJS::OperatorEQ(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorNE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorLT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetIntType(1, false),
            lhs->GetType()->IsSigned()
                ? builder.GetBuilder().CreateICmpSLT(lhs->Load(), rhs->Load())
                : builder.GetBuilder().CreateICmpULT(lhs->Load(), rhs->Load()));

    Error("TODO");
}

NJS::ValuePtr NJS::OperatorLE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetIntType(1, false),
            lhs->GetType()->IsSigned()
                ? builder.GetBuilder().CreateICmpSLE(lhs->Load(), rhs->Load())
                : builder.GetBuilder().CreateICmpULE(lhs->Load(), rhs->Load()));

    Error("TODO");
}

NJS::ValuePtr NJS::OperatorGT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorGE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorLOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorLXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorLAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorAdd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            builder.GetBuilder().CreateAdd(lhs->Load(), rhs->Load()));

    Error("TODO");
}

NJS::ValuePtr NJS::OperatorSub(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            lhs->GetType()->IsSigned()
                ? builder.GetBuilder().CreateSub(lhs->Load(), rhs->Load())
                : builder.GetBuilder().CreateSub(lhs->Load(), rhs->Load()));

    Error("TODO");
}

NJS::ValuePtr NJS::OperatorMul(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorDiv(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorRem(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorPow(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorShL(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::ValuePtr NJS::OperatorShR(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    Error("TODO");
}

NJS::TypePtr NJS::OperatorType(TypeContext& ctx, std::string op, const TypePtr& lhs, const TypePtr& rhs)
{
    if (op == "==" ||
        op == "!=" ||
        op == "<" ||
        op == ">" ||
        op == "<=" ||
        op == ">=")
        return ctx.GetIntType(1, false);

    if (op == "=")
        return lhs;

    if (op.back() == '=')
        op.pop_back();

    if (op == "+" ||
        op == "-" ||
        op == "*" ||
        op == "/" ||
        op == "%" ||
        op == "**" ||
        op == "|" ||
        op == "^" ||
        op == "&" ||
        op == "<<" ||
        op == ">>")
        return NJS::max(ctx, lhs, rhs);

    Error("TODO");
}

std::pair<NJS::ValuePtr, bool> NJS::OperatorInc(Builder& builder, const ValuePtr& val)
{
    if (val->GetType()->IsInt())
        return {
            RValue::Create(
                builder,
                val->GetType(),
                builder.GetBuilder().CreateAdd(
                    val->Load(),
                    llvm::ConstantInt::get(val->GetType()->GetLLVM(builder), 1, val->GetType()->IsSigned()))),
            true,
        };

    Error("TODO");
}

std::pair<NJS::ValuePtr, bool> NJS::OperatorDec(Builder& builder, const ValuePtr& val)
{
    Error("TODO");
}

std::pair<NJS::ValuePtr, bool> NJS::OperatorNeg(Builder& builder, const ValuePtr& val)
{
    Error("TODO");
}

std::pair<NJS::ValuePtr, bool> NJS::OperatorLNot(Builder& builder, const ValuePtr& val)
{
    Error("TODO");
}

std::pair<NJS::ValuePtr, bool> NJS::OperatorNot(Builder& builder, const ValuePtr& val)
{
    Error("TODO");
}
