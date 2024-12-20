#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::OperatorEQ(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetBoolType(),
            builder.GetBuilder().CreateICmpEQ(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorNE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    return {};
}

NJS::ValuePtr NJS::OperatorLT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetBoolType(),
            lhs->GetType()->IsSigned()
                ? builder.GetBuilder().CreateICmpSLT(lhs->Load(), rhs->Load())
                : builder.GetBuilder().CreateICmpULT(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorLE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetBoolType(),
            lhs->GetType()->IsSigned()
                ? builder.GetBuilder().CreateICmpSLE(lhs->Load(), rhs->Load())
                : builder.GetBuilder().CreateICmpULE(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorGT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetBoolType(),
            lhs->GetType()->IsSigned()
                ? builder.GetBuilder().CreateICmpSGT(lhs->Load(), rhs->Load())
                : builder.GetBuilder().CreateICmpUGT(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorGE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetBoolType(),
            lhs->GetType()->IsSigned()
                ? builder.GetBuilder().CreateICmpSGE(lhs->Load(), rhs->Load())
                : builder.GetBuilder().CreateICmpUGE(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorLOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    return RValue::Create(
        builder,
        builder.GetCtx().GetBoolType(),
        builder.GetBuilder().CreateOr(lhs->Load(), rhs->Load()));
}

NJS::ValuePtr NJS::OperatorLXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    return {};
}

NJS::ValuePtr NJS::OperatorLAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    return {};
}

NJS::ValuePtr NJS::OperatorOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            builder.GetBuilder().CreateOr(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            builder.GetBuilder().CreateXor(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            builder.GetBuilder().CreateAnd(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorAdd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            builder.GetBuilder().CreateAdd(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorSub(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            builder.GetBuilder().CreateSub(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorMul(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            builder.GetBuilder().CreateMul(lhs->Load(), rhs->Load()));

    if (lhs->GetType()->IsFP())
        return RValue::Create(
            builder,
            lhs->GetType(),
            builder.GetBuilder().CreateFMul(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorDiv(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            lhs->GetType()->IsSigned()
                ? builder.GetBuilder().CreateSDiv(lhs->Load(), rhs->Load())
                : builder.GetBuilder().CreateUDiv(lhs->Load(), rhs->Load()));

    if (lhs->GetType()->IsFP())
        return RValue::Create(
            builder,
            lhs->GetType(),
            builder.GetBuilder().CreateFDiv(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorRem(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            lhs->GetType()->IsSigned()
                ? builder.GetBuilder().CreateSRem(lhs->Load(), rhs->Load())
                : builder.GetBuilder().CreateURem(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorPow(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
    {
        const auto dst_ty = builder.GetCtx().GetFPType(lhs->GetType()->GetBits())->GetLLVM(builder);
        const auto src_ty = lhs->GetType()->GetLLVM(builder);
        const auto l = lhs->GetType()->IsSigned()
                           ? builder.GetBuilder().CreateSIToFP(lhs->Load(), dst_ty)
                           : builder.GetBuilder().CreateUIToFP(lhs->Load(), dst_ty);
        const auto r = lhs->GetType()->IsSigned()
                           ? builder.GetBuilder().CreateSIToFP(rhs->Load(), dst_ty)
                           : builder.GetBuilder().CreateUIToFP(rhs->Load(), dst_ty);
        const auto val = builder.GetBuilder().CreateBinaryIntrinsic(llvm::Intrinsic::pow, l, r);
        return RValue::Create(
            builder,
            lhs->GetType(),
            lhs->GetType()->IsSigned()
                ? builder.GetBuilder().CreateFPToSI(val, src_ty)
                : builder.GetBuilder().CreateFPToUI(val, src_ty));
    }

    return {};
}

NJS::ValuePtr NJS::OperatorShL(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            builder.GetBuilder().CreateShl(lhs->Load(), rhs->Load()));

    return {};
}

NJS::ValuePtr NJS::OperatorShR(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (lhs->GetType()->IsInt())
        return RValue::Create(
            builder,
            lhs->GetType(),
            lhs->GetType()->IsSigned()
                ? builder.GetBuilder().CreateAShr(lhs->Load(), rhs->Load())
                : builder.GetBuilder().CreateLShr(lhs->Load(), rhs->Load()));

    return {};
}

NJS::TypePtr NJS::OperatorType(TypeContext& ctx, std::string op, const TypePtr& lhs, const TypePtr& rhs)
{
    if (op == "==" ||
        op == "!=" ||
        op == "<" ||
        op == ">" ||
        op == "<=" ||
        op == ">=")
        return ctx.GetBoolType();

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
        op == ">>" ||
        op == "||" ||
        op == "^^" ||
        op == "&&")
        return NJS::max(ctx, lhs, rhs);

    return {};
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

    return {nullptr, false};
}

std::pair<NJS::ValuePtr, bool> NJS::OperatorDec(Builder& builder, const ValuePtr& val)
{
    if (val->GetType()->IsInt())
        return {
            RValue::Create(
                builder,
                val->GetType(),
                builder.GetBuilder().CreateSub(
                    val->Load(),
                    llvm::ConstantInt::get(val->GetType()->GetLLVM(builder), 1, val->GetType()->IsSigned()))),
            true,
        };

    return {nullptr, false};
}

std::pair<NJS::ValuePtr, bool> NJS::OperatorNeg(Builder& builder, const ValuePtr& val)
{
    if (val->GetType()->IsInt())
        return {
            RValue::Create(
                builder,
                val->GetType(),
                builder.GetBuilder().CreateNeg(val->Load())),
            false,
        };

    return {nullptr, false};
}

std::pair<NJS::ValuePtr, bool> NJS::OperatorLNot(Builder& builder, const ValuePtr& val)
{
    return {nullptr, false};
}

std::pair<NJS::ValuePtr, bool> NJS::OperatorNot(Builder& builder, const ValuePtr& val)
{
    if (val->GetType()->IsInt())
        return {
            RValue::Create(
                builder,
                val->GetType(),
                builder.GetBuilder().CreateNot(val->Load())),
            false,
        };

    return {nullptr, false};
}

std::pair<NJS::ValuePtr, bool> NJS::OperatorRef(Builder& builder, const ValuePtr& val)
{
    return {RValue::Create(builder, builder.GetCtx().GetPointerType(val->GetType()), val->GetPtr()), false};
}
