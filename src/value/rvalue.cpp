#include <NJS/Error.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::RValue::Create(Builder& builder, const TypePtr& type, llvm::Value* value)
{
    return std::shared_ptr<RValue>(new RValue(builder, type, value));
}

bool NJS::RValue::IsL() const
{
    return false;
}

llvm::Value* NJS::RValue::GetPtr() const
{
    Error("cannot get pointer to rvalue");
}

llvm::Value* NJS::RValue::Load() const
{
    return m_Value;
}

void NJS::RValue::Store(llvm::Value*) const
{
    Error("cannot assign to rvalue");
}

void NJS::RValue::Store(ValuePtr) const
{
    Error("cannot assign to rvalue");
}

NJS::RValue::RValue(Builder& builder, TypePtr type, llvm::Value* value)
    : Value(builder, std::move(type)), m_Value(value)
{
}
