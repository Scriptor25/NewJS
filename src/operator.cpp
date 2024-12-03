#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::OperatorEQ(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFCmpOEQ(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorNE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFCmpONE(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFCmpOLT(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFCmpOLE(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorGT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFCmpOGT(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorGE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFCmpOGE(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.LLVMBuilder().CreateIsNotNull(lhs->Load());
    const auto r = builder.LLVMBuilder().CreateIsNotNull(rhs->Load());
    const auto value = builder.LLVMBuilder().CreateOr(l, r);
    return RValue::Create(builder, builder.Ctx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.LLVMBuilder().CreateIsNotNull(lhs->Load());
    const auto r = builder.LLVMBuilder().CreateIsNotNull(rhs->Load());
    const auto value = builder.LLVMBuilder().CreateXor(l, r);
    return RValue::Create(builder, builder.Ctx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.LLVMBuilder().CreateIsNotNull(lhs->Load());
    const auto r = builder.LLVMBuilder().CreateIsNotNull(rhs->Load());
    const auto value = builder.LLVMBuilder().CreateAnd(l, r);
    return RValue::Create(builder, builder.Ctx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.LLVMBuilder().CreateFPToSI(lhs->Load(), builder.LLVMBuilder().getInt64Ty());
    const auto r = builder.LLVMBuilder().CreateFPToSI(rhs->Load(), builder.LLVMBuilder().getInt64Ty());
    const auto value = builder.LLVMBuilder().CreateOr(l, r);
    const auto result = builder.LLVMBuilder().CreateSIToFP(value, builder.LLVMBuilder().getDoubleTy());
    return RValue::Create(builder, builder.Ctx().GetNumberType(), result);
}

NJS::ValuePtr NJS::OperatorXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.LLVMBuilder().CreateFPToSI(lhs->Load(), builder.LLVMBuilder().getInt64Ty());
    const auto r = builder.LLVMBuilder().CreateFPToSI(rhs->Load(), builder.LLVMBuilder().getInt64Ty());
    const auto value = builder.LLVMBuilder().CreateXor(l, r);
    const auto result = builder.LLVMBuilder().CreateSIToFP(value, builder.LLVMBuilder().getDoubleTy());
    return RValue::Create(builder, builder.Ctx().GetNumberType(), result);
}

NJS::ValuePtr NJS::OperatorAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.LLVMBuilder().CreateFPToSI(lhs->Load(), builder.LLVMBuilder().getInt64Ty());
    const auto r = builder.LLVMBuilder().CreateFPToSI(rhs->Load(), builder.LLVMBuilder().getInt64Ty());
    const auto value = builder.LLVMBuilder().CreateAnd(l, r);
    const auto result = builder.LLVMBuilder().CreateSIToFP(value, builder.LLVMBuilder().getDoubleTy());
    return RValue::Create(builder, builder.Ctx().GetNumberType(), result);
}

NJS::ValuePtr NJS::OperatorAdd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFAdd(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorSub(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFSub(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorMul(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFMul(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorDiv(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFDiv(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorRem(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFRem(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorPow(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateBinaryIntrinsic(llvm::Intrinsic::pow, lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorRoot(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto one = llvm::ConstantFP::get(builder.LLVMBuilder().getDoubleTy(), 1.0);
    const auto r = builder.LLVMBuilder().CreateFDiv(one, rhs->Load());
    const auto value = builder.LLVMBuilder().CreateBinaryIntrinsic(llvm::Intrinsic::pow, lhs->Load(), r);
    return RValue::Create(builder, builder.Ctx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorShL(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.LLVMBuilder().CreateFPToSI(lhs->Load(), builder.LLVMBuilder().getInt64Ty());
    const auto r = builder.LLVMBuilder().CreateFPToSI(rhs->Load(), builder.LLVMBuilder().getInt64Ty());
    const auto value = builder.LLVMBuilder().CreateShl(l, r);
    const auto result = builder.LLVMBuilder().CreateSIToFP(value, builder.LLVMBuilder().getDoubleTy());
    return RValue::Create(builder, builder.Ctx().GetNumberType(), result);
}

NJS::ValuePtr NJS::OperatorShR(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.LLVMBuilder().CreateFPToSI(lhs->Load(), builder.LLVMBuilder().getInt64Ty());
    const auto r = builder.LLVMBuilder().CreateFPToSI(rhs->Load(), builder.LLVMBuilder().getInt64Ty());
    const auto value = builder.LLVMBuilder().CreateAShr(l, r);
    const auto result = builder.LLVMBuilder().CreateSIToFP(value, builder.LLVMBuilder().getDoubleTy());
    return RValue::Create(builder, builder.Ctx().GetNumberType(), result);
}
