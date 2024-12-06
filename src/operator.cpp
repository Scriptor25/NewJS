#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::OperatorEQ(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();
    llvm::Value* value;

    if (type->IsPrimitive(Primitive_Number))
        value = builder.GetBuilder().CreateFCmpOEQ(lhs->Load(), rhs->Load());
    else if (type->IsPrimitive(Primitive_Boolean) || type->IsPrimitive(Primitive_Char))
        value = builder.GetBuilder().CreateICmpEQ(lhs->Load(), rhs->Load());
    else return {};

    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorNE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();
    llvm::Value* value;

    if (type->IsPrimitive(Primitive_Number))
        value = builder.GetBuilder().CreateFCmpONE(lhs->Load(), rhs->Load());
    else if (type->IsPrimitive(Primitive_Boolean) || type->IsPrimitive(Primitive_Char))
        value = builder.GetBuilder().CreateICmpNE(lhs->Load(), rhs->Load());
    else return {};

    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFCmpOLT(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFCmpOLE(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorGT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFCmpOGT(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorGE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFCmpOGE(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Boolean))
        return {};

    const auto value = builder.GetBuilder().CreateOr(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Boolean))
        return {};

    const auto value = builder.GetBuilder().CreateXor(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Boolean))
        return {};

    const auto value = builder.GetBuilder().CreateAnd(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto l = builder.GetBuilder().CreateFPToSI(lhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto r = builder.GetBuilder().CreateFPToSI(rhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto result = builder.GetBuilder().CreateOr(l, r);
    const auto value = builder.GetBuilder().CreateSIToFP(result, builder.GetBuilder().getDoubleTy());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto l = builder.GetBuilder().CreateFPToSI(lhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto r = builder.GetBuilder().CreateFPToSI(rhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto result = builder.GetBuilder().CreateXor(l, r);
    const auto value = builder.GetBuilder().CreateSIToFP(result, builder.GetBuilder().getDoubleTy());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto l = builder.GetBuilder().CreateFPToSI(lhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto r = builder.GetBuilder().CreateFPToSI(rhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto result = builder.GetBuilder().CreateAnd(l, r);
    const auto value = builder.GetBuilder().CreateSIToFP(result, builder.GetBuilder().getDoubleTy());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorAdd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFAdd(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorSub(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFSub(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorMul(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFMul(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorDiv(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFDiv(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorRem(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFRem(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorPow(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateBinaryIntrinsic(llvm::Intrinsic::pow, lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorRoot(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto one = llvm::ConstantFP::get(builder.GetBuilder().getDoubleTy(), 1.0);
    const auto r = builder.GetBuilder().CreateFDiv(one, rhs->Load());
    const auto value = builder.GetBuilder().CreateBinaryIntrinsic(llvm::Intrinsic::pow, lhs->Load(), r);
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorShL(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto l = builder.GetBuilder().CreateFPToSI(lhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto r = builder.GetBuilder().CreateFPToSI(rhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto result = builder.GetBuilder().CreateShl(l, r);
    const auto value = builder.GetBuilder().CreateSIToFP(result, builder.GetBuilder().getDoubleTy());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorShR(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto l = builder.GetBuilder().CreateFPToSI(lhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto r = builder.GetBuilder().CreateFPToSI(rhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto result = builder.GetBuilder().CreateAShr(l, r);
    const auto value = builder.GetBuilder().CreateSIToFP(result, builder.GetBuilder().getDoubleTy());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}
