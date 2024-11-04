#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::OperatorLE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto value = builder.LLVMBuilder().CreateFCmpOLE(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.Ctx().GetBooleanType(), value);
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
