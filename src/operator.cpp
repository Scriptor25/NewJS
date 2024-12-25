#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::OperatorEQ(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetBoolType(),
            builder.GetBuilder().CreateICmpEQ(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorNE(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetBoolType(),
            builder.GetBuilder().CreateICmpNE(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorLT(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetBoolType(),
            type->IsSigned()
                ? builder.GetBuilder().CreateICmpSLT(lhs, rhs)
                : builder.GetBuilder().CreateICmpULT(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorLE(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetBoolType(),
            type->IsSigned()
                ? builder.GetBuilder().CreateICmpSLE(lhs, rhs)
                : builder.GetBuilder().CreateICmpULE(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorGT(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetBoolType(),
            type->IsSigned()
                ? builder.GetBuilder().CreateICmpSGT(lhs, rhs)
                : builder.GetBuilder().CreateICmpUGT(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorGE(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            builder.GetCtx().GetBoolType(),
            type->IsSigned()
                ? builder.GetBuilder().CreateICmpSGE(lhs, rhs)
                : builder.GetBuilder().CreateICmpUGE(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorLOr(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (!type->IsInt() || !type->GetBits() == 1)
        return {};

    return RValue::Create(
        builder,
        builder.GetCtx().GetBoolType(),
        builder.GetBuilder().CreateOr(lhs, rhs));
}

NJS::ValuePtr NJS::OperatorLXor(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (!type->IsInt() || !type->GetBits() == 1)
        return {};

    return RValue::Create(
        builder,
        builder.GetCtx().GetBoolType(),
        builder.GetBuilder().CreateXor(lhs, rhs));
}

NJS::ValuePtr NJS::OperatorLAnd(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (!type->IsInt() || !type->GetBits() == 1)
        return {};

    return RValue::Create(
        builder,
        builder.GetCtx().GetBoolType(),
        builder.GetBuilder().CreateAnd(lhs, rhs));
}

NJS::ValuePtr NJS::OperatorOr(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateOr(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorXor(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateXor(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorAnd(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateAnd(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorAdd(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateAdd(lhs, rhs));

    if (type->IsFP())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFAdd(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorSub(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateSub(lhs, rhs));


    if (type->IsFP())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFSub(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorMul(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateMul(lhs, rhs));

    if (type->IsFP())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFMul(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorDiv(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            type,
            type->IsSigned()
                ? builder.GetBuilder().CreateSDiv(lhs, rhs)
                : builder.GetBuilder().CreateUDiv(lhs, rhs));

    if (type->IsFP())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFDiv(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorRem(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            type,
            type->IsSigned()
                ? builder.GetBuilder().CreateSRem(lhs, rhs)
                : builder.GetBuilder().CreateURem(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorPow(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
    {
        const auto dst_ty = builder.GetCtx().GetFPType(type->GetBits())->GetLLVM(builder);
        const auto src_ty = type->GetLLVM(builder);
        const auto l = type->IsSigned()
                           ? builder.GetBuilder().CreateSIToFP(lhs, dst_ty)
                           : builder.GetBuilder().CreateUIToFP(lhs, dst_ty);
        const auto r = type->IsSigned()
                           ? builder.GetBuilder().CreateSIToFP(rhs, dst_ty)
                           : builder.GetBuilder().CreateUIToFP(rhs, dst_ty);
        const auto val = builder.GetBuilder().CreateBinaryIntrinsic(llvm::Intrinsic::pow, l, r);
        return RValue::Create(
            builder,
            type,
            type->IsSigned()
                ? builder.GetBuilder().CreateFPToSI(val, src_ty)
                : builder.GetBuilder().CreateFPToUI(val, src_ty));
    }

    return {};
}

NJS::ValuePtr NJS::OperatorShL(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateShl(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorShR(Builder& builder, const TypePtr& type, llvm::Value* lhs, llvm::Value* rhs)
{
    if (type->IsInt())
        return RValue::Create(
            builder,
            type,
            type->IsSigned()
                ? builder.GetBuilder().CreateAShr(lhs, rhs)
                : builder.GetBuilder().CreateLShr(lhs, rhs));

    return {};
}

NJS::UnaryResult NJS::OperatorInc(Builder& builder, const SourceLocation&, const ValuePtr& val)
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

NJS::UnaryResult NJS::OperatorDec(Builder& builder, const SourceLocation&, const ValuePtr& val)
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

NJS::UnaryResult NJS::OperatorNeg(Builder& builder, const SourceLocation&, const ValuePtr& val)
{
    if (val->GetType()->IsInt())
        return {
            RValue::Create(
                builder,
                val->GetType(),
                builder.GetBuilder().CreateNeg(val->Load())),
            false,
        };

    if (val->GetType()->IsFP())
        return {
            RValue::Create(
                builder,
                val->GetType(),
                builder.GetBuilder().CreateFNeg(val->Load())),
            false,
        };

    return {nullptr, false};
}

NJS::UnaryResult NJS::OperatorLNot(Builder&, const SourceLocation&, const ValuePtr&)
{
    return {nullptr, false};
}

NJS::UnaryResult NJS::OperatorNot(Builder& builder, const SourceLocation&, const ValuePtr& val)
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

NJS::UnaryResult NJS::OperatorRef(Builder& builder, const SourceLocation& where, const ValuePtr& val)
{
    return {RValue::Create(builder, builder.GetCtx().GetPointerType(val->GetType()), val->GetPtr(where)), false};
}
