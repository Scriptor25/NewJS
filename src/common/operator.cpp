#include <newjs/builder.hpp>
#include <newjs/operator.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::OperatorEQ(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateICmpEQ(lhs, rhs));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpOEQ(lhs, rhs));

    if (type->IsPointer() || type->IsFunction())
    {
        const auto lhs_int = builder.GetBuilder().CreatePtrToInt(lhs, builder.GetBuilder().getInt64Ty());
        const auto rhs_int = builder.GetBuilder().CreatePtrToInt(rhs, builder.GetBuilder().getInt64Ty());
        return OperatorEQ(builder, builder.GetTypeContext().GetIntegerType(64, false), lhs_int, rhs_int);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorNE(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateICmpNE(lhs, rhs));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpONE(lhs, rhs));

    if (type->IsPointer() || type->IsFunction())
    {
        const auto lhs_int = builder.GetBuilder().CreatePtrToInt(lhs, builder.GetBuilder().getInt64Ty());
        const auto rhs_int = builder.GetBuilder().CreatePtrToInt(rhs, builder.GetBuilder().getInt64Ty());
        return OperatorNE(builder, builder.GetTypeContext().GetIntegerType(64, false), lhs_int, rhs_int);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLT(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            Type::As<IntegerType>(type)->IsSigned()
                ? builder.GetBuilder().CreateICmpSLT(lhs, rhs)
                : builder.GetBuilder().CreateICmpULT(lhs, rhs));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpOLT(lhs, rhs));

    if (type->IsPointer() || type->IsFunction())
    {
        const auto lhs_int = builder.GetBuilder().CreatePtrToInt(lhs, builder.GetBuilder().getInt64Ty());
        const auto rhs_int = builder.GetBuilder().CreatePtrToInt(rhs, builder.GetBuilder().getInt64Ty());
        return OperatorLT(builder, builder.GetTypeContext().GetIntegerType(64, false), lhs_int, rhs_int);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLE(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            Type::As<IntegerType>(type)->IsSigned()
                ? builder.GetBuilder().CreateICmpSLE(lhs, rhs)
                : builder.GetBuilder().CreateICmpULE(lhs, rhs));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpOLE(lhs, rhs));

    if (type->IsPointer() || type->IsFunction())
    {
        const auto lhs_int = builder.GetBuilder().CreatePtrToInt(lhs, builder.GetBuilder().getInt64Ty());
        const auto rhs_int = builder.GetBuilder().CreatePtrToInt(rhs, builder.GetBuilder().getInt64Ty());
        return OperatorLE(builder, builder.GetTypeContext().GetIntegerType(64, false), lhs_int, rhs_int);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorGT(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            Type::As<IntegerType>(type)->IsSigned()
                ? builder.GetBuilder().CreateICmpSGT(lhs, rhs)
                : builder.GetBuilder().CreateICmpUGT(lhs, rhs));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpOGT(lhs, rhs));

    if (type->IsPointer() || type->IsFunction())
    {
        const auto lhs_int = builder.GetBuilder().CreatePtrToInt(lhs, builder.GetBuilder().getInt64Ty());
        const auto rhs_int = builder.GetBuilder().CreatePtrToInt(rhs, builder.GetBuilder().getInt64Ty());
        return OperatorGT(builder, builder.GetTypeContext().GetIntegerType(64, false), lhs_int, rhs_int);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorGE(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            Type::As<IntegerType>(type)->IsSigned()
                ? builder.GetBuilder().CreateICmpSGE(lhs, rhs)
                : builder.GetBuilder().CreateICmpUGE(lhs, rhs));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpOGE(lhs, rhs));

    if (type->IsPointer() || type->IsFunction())
    {
        const auto lhs_int = builder.GetBuilder().CreatePtrToInt(lhs, builder.GetBuilder().getInt64Ty());
        const auto rhs_int = builder.GetBuilder().CreatePtrToInt(rhs, builder.GetBuilder().getInt64Ty());
        return OperatorGE(builder, builder.GetTypeContext().GetIntegerType(64, false), lhs_int, rhs_int);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLOr(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger() && Type::As<IntegerType>(type)->GetBits() == 1)
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateOr(lhs, rhs));

    if (type->IsInteger())
    {
        const auto lhs_not_null = builder.GetBuilder().CreateIsNotNull(lhs);
        const auto rhs_not_null = builder.GetBuilder().CreateIsNotNull(rhs);
        return OperatorLOr(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            lhs_not_null,
            rhs_not_null);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLXor(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger() && Type::As<IntegerType>(type)->GetBits() == 1)
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateXor(lhs, rhs));

    if (type->IsInteger())
    {
        const auto lhs_not_null = builder.GetBuilder().CreateIsNotNull(lhs);
        const auto rhs_not_null = builder.GetBuilder().CreateIsNotNull(rhs);
        return OperatorLXor(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            lhs_not_null,
            rhs_not_null);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLAnd(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger() && Type::As<IntegerType>(type)->GetBits() == 1)
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateAnd(lhs, rhs));

    if (type->IsInteger())
    {
        const auto lhs_not_null = builder.GetBuilder().CreateIsNotNull(lhs);
        const auto rhs_not_null = builder.GetBuilder().CreateIsNotNull(rhs);
        return OperatorLAnd(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            lhs_not_null,
            rhs_not_null);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorOr(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateOr(lhs, rhs));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorXor(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateXor(lhs, rhs));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorAnd(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateAnd(lhs, rhs));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorAdd(
    Builder &builder,
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

    return nullptr;
}

NJS::ValuePtr NJS::OperatorSub(
    Builder &builder,
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
            builder.GetBuilder().CreatePtrDiff(
                Type::As<PointerType>(type)->GetElement()->GetLLVM(builder),
                lhs,
                rhs));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorMul(
    Builder &builder,
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

    return nullptr;
}

NJS::ValuePtr NJS::OperatorDiv(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            Type::As<IntegerType>(type)->IsSigned()
                ? builder.GetBuilder().CreateSDiv(lhs, rhs)
                : builder.GetBuilder().CreateUDiv(lhs, rhs));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFDiv(lhs, rhs));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorRem(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            Type::As<IntegerType>(type)->IsSigned()
                ? builder.GetBuilder().CreateSRem(lhs, rhs)
                : builder.GetBuilder().CreateURem(lhs, rhs));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorPow(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
    {
        const auto dst_ty = builder
                .GetTypeContext()
                .GetFloatingPointType(Type::As<IntegerType>(type)->GetBits())
                ->GetLLVM(builder);
        const auto src_ty = type->GetLLVM(builder);
        const auto lhs_fp = Type::As<IntegerType>(type)->IsSigned()
                                ? builder.GetBuilder().CreateSIToFP(lhs, dst_ty)
                                : builder.GetBuilder().CreateUIToFP(lhs, dst_ty);
        const auto rhs_fp = Type::As<IntegerType>(type)->IsSigned()
                                ? builder.GetBuilder().CreateSIToFP(rhs, dst_ty)
                                : builder.GetBuilder().CreateUIToFP(rhs, dst_ty);
        const auto val = builder.GetBuilder().CreateBinaryIntrinsic(llvm::Intrinsic::pow, lhs_fp, rhs_fp);
        return RValue::Create(
            builder,
            type,
            Type::As<IntegerType>(type)->IsSigned()
                ? builder.GetBuilder().CreateFPToSI(val, src_ty)
                : builder.GetBuilder().CreateFPToUI(val, src_ty));
    }

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateBinaryIntrinsic(llvm::Intrinsic::pow, lhs, rhs));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorShL(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateShl(lhs, rhs));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorShR(
    Builder &builder,
    const TypePtr &type,
    llvm::Value *lhs,
    llvm::Value *rhs)
{
    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            Type::As<IntegerType>(type)->IsSigned()
                ? builder.GetBuilder().CreateAShr(lhs, rhs)
                : builder.GetBuilder().CreateLShr(lhs, rhs));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorInc(Builder &builder, const ValuePtr &value)
{
    const auto val = value->Load();
    const auto type = value->GetType();
    const auto ty = type->GetLLVM(builder);

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateAdd(
                val,
                llvm::ConstantInt::get(ty, 1, Type::As<IntegerType>(type)->IsSigned())));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFAdd(val, llvm::ConstantFP::get(ty, 1.0)));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorDec(Builder &builder, const ValuePtr &value)
{
    const auto val = value->Load();
    const auto type = value->GetType();
    const auto ty = type->GetLLVM(builder);

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateSub(
                val,
                llvm::ConstantInt::get(ty, 1, Type::As<IntegerType>(type)->IsSigned())));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFSub(val, llvm::ConstantFP::get(ty, 1.0)));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorNeg(Builder &builder, const ValuePtr &value)
{
    const auto val = value->Load();
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

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLNot(Builder &builder, const ValuePtr &value)
{
    const auto val = value->Load();
    const auto type = value->GetType();

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateIsNull(val));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorNot(Builder &builder, const ValuePtr &value)
{
    const auto val = value->Load();
    const auto type = value->GetType();

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateNot(val));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorRef(Builder &builder, const ValuePtr &value)
{
    const auto ptr = value->GetPointer();
    const auto type = value->GetType();

    return RValue::Create(
        builder,
        builder.GetTypeContext().GetPointerType(type, value->IsConst()),
        ptr);
}

NJS::ValuePtr NJS::OperatorDeref(Builder &builder, const ValuePtr &value)
{
    const auto val = value->Load();
    const auto type = value->GetType();

    if (type->IsPointer())
        return LValue::Create(
            builder,
            Type::As<PointerType>(type)->GetElement(),
            val,
            Type::As<PointerType>(type)->IsConst());

    return nullptr;
}
