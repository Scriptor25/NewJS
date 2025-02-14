#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::OperatorEQ(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            builder.GetBuilder().CreateICmpEQ(lhs, rhs));
    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            builder.GetBuilder().CreateFCmpOEQ(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorNE(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            builder.GetBuilder().CreateICmpNE(lhs, rhs));
    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            builder.GetBuilder().CreateFCmpONE(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorLT(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            type->IsSigned()
                ? builder.GetBuilder().CreateICmpSLT(lhs, rhs)
                : builder.GetBuilder().CreateICmpULT(lhs, rhs));
    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            builder.GetBuilder().CreateFCmpOLT(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorLE(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            type->IsSigned()
                ? builder.GetBuilder().CreateICmpSLE(lhs, rhs)
                : builder.GetBuilder().CreateICmpULE(lhs, rhs));
    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            builder.GetBuilder().CreateFCmpOLE(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorGT(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            type->IsSigned()
                ? builder.GetBuilder().CreateICmpSGT(lhs, rhs)
                : builder.GetBuilder().CreateICmpUGT(lhs, rhs));
    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            builder.GetBuilder().CreateFCmpOGT(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorGE(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            type->IsSigned()
                ? builder.GetBuilder().CreateICmpSGE(lhs, rhs)
                : builder.GetBuilder().CreateICmpUGE(lhs, rhs));
    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            builder.GetBuilder().CreateFCmpOGE(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorLOr(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger() && type->GetBits() == 1)
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            builder.GetBuilder().CreateOr(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorLXor(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger() && type->GetBits() == 1)
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            builder.GetBuilder().CreateXor(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorLAnd(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger() && type->GetBits() == 1)
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBoolType(),
            builder.GetBuilder().CreateAnd(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorOr(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateOr(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorXor(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateXor(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorAnd(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateAnd(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorAdd(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateAdd(lhs, rhs));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFAdd(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorSub(
    Builder &builder,
    const SourceLocation &where,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateSub(lhs, rhs));


    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFSub(lhs, rhs));

    if (type->IsPointer())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetIntegerType(64, true),
            builder.GetBuilder().CreatePtrDiff(type->GetElement()->GetLLVM(where, builder), lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorMul(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateMul(lhs, rhs));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFMul(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorDiv(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            type->IsSigned()
                ? builder.GetBuilder().CreateSDiv(lhs, rhs)
                : builder.GetBuilder().CreateUDiv(lhs, rhs));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFDiv(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorRem(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            type->IsSigned()
                ? builder.GetBuilder().CreateSRem(lhs, rhs)
                : builder.GetBuilder().CreateURem(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorPow(
    Builder &builder,
    const SourceLocation &where,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
    {
        const auto dst_ty = builder.GetTypeContext().GetFloatingPointType(type->GetBits())->GetLLVM(where, builder);
        const auto src_ty = type->GetLLVM(where, builder);
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

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateBinaryIntrinsic(llvm::Intrinsic::pow, lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorShL(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateShl(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorShR(
    Builder &builder,
    const SourceLocation &,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            type->IsSigned()
                ? builder.GetBuilder().CreateAShr(lhs, rhs)
                : builder.GetBuilder().CreateLShr(lhs, rhs));

    return {};
}

NJS::ValuePtr NJS::OperatorInc(Builder &builder, const SourceLocation &where, const ValuePtr &value)
{
    const auto val = value->Load(where);
    const auto type = value->GetType();
    const auto ty = type->GetLLVM(where, builder);

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateAdd(val, llvm::ConstantInt::get(ty, 1, type->IsSigned())));
    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFAdd(val, llvm::ConstantFP::get(ty, 1.0)));

    return {};
}

NJS::ValuePtr NJS::OperatorDec(Builder &builder, const SourceLocation &where, const ValuePtr &value)
{
    const auto val = value->Load(where);
    const auto type = value->GetType();
    const auto ty = type->GetLLVM(where, builder);

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateSub(val, llvm::ConstantInt::get(ty, 1, type->IsSigned())));
    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFSub(val, llvm::ConstantFP::get(ty, 1.0)));

    return {};
}

NJS::ValuePtr NJS::OperatorNeg(Builder &builder, const SourceLocation &where, const ValuePtr &value)
{
    const auto val = value->Load(where);
    const auto type = value->GetType();

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateNeg(val));
    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFNeg(val));

    return {};
}

NJS::ValuePtr NJS::OperatorLNot(Builder &, const SourceLocation &, const ValuePtr &)
{
    return {};
}

NJS::ValuePtr NJS::OperatorNot(Builder &builder, const SourceLocation &where, const ValuePtr &value)
{
    const auto val = value->Load(where);
    const auto type = value->GetType();

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateNot(val));

    return {};
}

NJS::ValuePtr NJS::OperatorRef(Builder &builder, const SourceLocation &where, const ValuePtr &value)
{
    const auto ptr = value->GetPtr(where);
    const auto type = value->GetType();

    return RValue::Create(
        builder,
        builder.GetTypeContext().GetPointerType(type),
        ptr);
}

NJS::ValuePtr NJS::OperatorDeref(Builder &builder, const SourceLocation &where, const ValuePtr &value)
{
    const auto val = value->Load(where);
    const auto type = value->GetType();

    return LValue::Create(
        builder,
        type->GetElement(),
        val);
}
