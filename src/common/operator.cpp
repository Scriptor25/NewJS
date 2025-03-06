#include <newjs/builder.hpp>
#include <newjs/operator.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::OperatorEQ(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsIntegerLike())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateICmpEQ(left_value, right_value));

    if (left_type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpOEQ(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorNE(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsIntegerLike())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateICmpNE(left_value, right_value));

    if (left_type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpONE(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLT(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            Type::As<IntegerType>(left_type)->IsSigned()
                ? builder.GetBuilder().CreateICmpSLT(left_value, right_value)
                : builder.GetBuilder().CreateICmpULT(left_value, right_value));

    if (left_type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpOLT(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLE(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            Type::As<IntegerType>(left_type)->IsSigned()
                ? builder.GetBuilder().CreateICmpSLE(left_value, right_value)
                : builder.GetBuilder().CreateICmpULE(left_value, right_value));

    if (left_type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpOLE(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorGT(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            Type::As<IntegerType>(left_type)->IsSigned()
                ? builder.GetBuilder().CreateICmpSGT(left_value, right_value)
                : builder.GetBuilder().CreateICmpUGT(left_value, right_value));

    if (left_type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpOGT(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorGE(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            Type::As<IntegerType>(left_type)->IsSigned()
                ? builder.GetBuilder().CreateICmpSGE(left_value, right_value)
                : builder.GetBuilder().CreateICmpUGE(left_value, right_value));

    if (left_type->IsFloatingPoint())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateFCmpOGE(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLOr(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type->IsBoolean() && right_type->IsBoolean())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateOr(left_value, right_value));

    if (left_type->IsIntegerLike() && right_type->IsIntegerLike())
    {
        const auto lhs_not_null = RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateIsNotNull(left_value));
        const auto rhs_not_null = RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateIsNotNull(right_value));
        return OperatorLOr(builder, lhs_not_null, rhs_not_null);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLXor(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type->IsBoolean() && right_type->IsBoolean())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateXor(left_value, right_value));

    if (left_type->IsIntegerLike() && right_type->IsIntegerLike())
    {
        const auto lhs_not_null = RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateIsNotNull(left_value));
        const auto rhs_not_null = RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateIsNotNull(right_value));
        return OperatorLXor(builder, lhs_not_null, rhs_not_null);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLAnd(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type->IsBoolean() && right_type->IsBoolean())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateAnd(left_value, right_value));

    if (left_type->IsIntegerLike() && right_type->IsIntegerLike())
    {
        const auto lhs_not_null = RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateIsNotNull(left_value));
        const auto rhs_not_null = RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateIsNotNull(right_value));
        return OperatorLAnd(builder, lhs_not_null, rhs_not_null);
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorOr(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateOr(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorXor(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateXor(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorAnd(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateAnd(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorAdd(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type->IsPointer() && right_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreatePtrAdd(left_value, right_value));

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateAdd(left_value, right_value));

    if (left_type->IsFloatingPoint())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateFAdd(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorSub(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type->IsPointer() && right_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreatePtrAdd(left_value, builder.GetBuilder().CreateNeg(right_value)));

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateSub(left_value, right_value));

    if (left_type->IsFloatingPoint())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateFSub(left_value, right_value));

    if (left_type->IsPointer())
        return RValue::Create(
            builder,
            builder.GetTypeContext().GetIntegerType(64, true),
            builder.GetBuilder().CreatePtrDiff(
                Type::As<PointerType>(left_type)->GetElement()->GetLLVM(builder),
                left_value,
                right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorMul(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateMul(left_value, right_value));

    if (left_type->IsFloatingPoint())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateFMul(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorDiv(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            Type::As<IntegerType>(left_type)->IsSigned()
                ? builder.GetBuilder().CreateSDiv(left_value, right_value)
                : builder.GetBuilder().CreateUDiv(left_value, right_value));

    if (left_type->IsFloatingPoint())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateFDiv(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorRem(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            Type::As<IntegerType>(left_type)->IsSigned()
                ? builder.GetBuilder().CreateSRem(left_value, right_value)
                : builder.GetBuilder().CreateURem(left_value, right_value));

    if (left_type->IsFloatingPoint())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateFRem(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorPow(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type->IsInteger())
    {
        const auto left_fp = builder.CreateCast(
            left,
            builder.GetTypeContext().GetFloatingPointType(Type::As<IntegerType>(left_type)->GetBits()));
        return OperatorPow(builder, left_fp, right);
    }

    if (right_type->IsInteger())
    {
        const auto right_fp = builder.CreateCast(
            right,
            builder.GetTypeContext().GetFloatingPointType(Type::As<IntegerType>(right_type)->GetBits()));
        return OperatorPow(builder, left, right_fp);
    }

    if (left_type->IsFloatingPoint() && right_type->IsFloatingPoint())
    {
        if (left_type != right_type)
        {
            const auto type = GetHigherOrderOf(builder.GetTypeContext(), left_type, right_type);
            const auto left_cast = builder.CreateCast(left, type);
            const auto right_cast = builder.CreateCast(right, type);
            return OperatorPow(builder, left_cast, right_cast);
        }

        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateBinaryIntrinsic(llvm::Intrinsic::pow, left_value, right_value));
    }

    return nullptr;
}

NJS::ValuePtr NJS::OperatorShL(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            builder.GetBuilder().CreateShl(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorShR(
    Builder &builder,
    const ValuePtr &left,
    const ValuePtr &right)
{
    const auto left_type = left->GetType();
    const auto right_type = right->GetType();
    const auto left_value = left->Load();
    const auto right_value = right->Load();

    if (left_type != right_type)
        return nullptr;

    if (left_type->IsInteger())
        return RValue::Create(
            builder,
            left_type,
            Type::As<IntegerType>(left_type)->IsSigned()
                ? builder.GetBuilder().CreateAShr(left_value, right_value)
                : builder.GetBuilder().CreateLShr(left_value, right_value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorInc(Builder &builder, const ValuePtr &operand)
{
    const auto value = operand->Load();
    const auto type = operand->GetType();
    const auto ty = type->GetLLVM(builder);

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateAdd(
                value,
                llvm::ConstantInt::get(ty, 1, Type::As<IntegerType>(type)->IsSigned())));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFAdd(value, llvm::ConstantFP::get(ty, 1.0)));

    if (type->IsPointer())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreatePtrAdd(
                value,
                llvm::ConstantInt::get(ty, 1, false)));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorDec(Builder &builder, const ValuePtr &operand)
{
    const auto value = operand->Load();
    const auto type = operand->GetType();
    const auto ty = type->GetLLVM(builder);

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateSub(
                value,
                llvm::ConstantInt::get(ty, 1, Type::As<IntegerType>(type)->IsSigned())));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFSub(value, llvm::ConstantFP::get(ty, 1.0)));

    if (type->IsPointer())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreatePtrAdd(
                value,
                llvm::ConstantInt::get(ty, -1, true)));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorNeg(Builder &builder, const ValuePtr &operand)
{
    const auto value = operand->Load();
    const auto type = operand->GetType();

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateNeg(value));

    if (type->IsFloatingPoint())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateFNeg(value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorLNot(Builder &builder, const ValuePtr &operand)
{
    const auto value = operand->Load();
    const auto type = operand->GetType();

    if (type->IsIntegerLike())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateIsNull(value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorNot(Builder &builder, const ValuePtr &operand)
{
    const auto value = operand->Load();
    const auto type = operand->GetType();

    if (type->IsInteger())
        return RValue::Create(
            builder,
            type,
            builder.GetBuilder().CreateNot(value));

    return nullptr;
}

NJS::ValuePtr NJS::OperatorRef(Builder &builder, const ValuePtr &operand)
{
    const auto pointer = operand->GetPointer();
    const auto type = operand->GetType();

    return RValue::Create(
        builder,
        builder.GetTypeContext().GetPointerType(type, operand->IsConst()),
        pointer);
}

NJS::ValuePtr NJS::OperatorDeref(Builder &builder, const ValuePtr &operand)
{
    const auto value = operand->Load();
    const auto type = operand->GetType();

    if (type->IsPointer())
        return LValue::Create(
            builder,
            Type::As<PointerType>(type)->GetElement(),
            value,
            Type::As<PointerType>(type)->IsConst());

    return nullptr;
}
