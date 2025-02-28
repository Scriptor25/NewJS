#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::RValue::Create(Builder &builder, const TypePtr &type, llvm::Value *value)
{
    return std::shared_ptr<RValue>(new RValue(builder, type, value));
}

bool NJS::RValue::IsLValue() const
{
    return false;
}

bool NJS::RValue::IsConst() const
{
    return true;
}

llvm::Value *NJS::RValue::GetPointer() const
{
    return nullptr;
}

llvm::Value *NJS::RValue::Load() const
{
    return m_Value;
}

void NJS::RValue::Store(llvm::Value *) const
{
}

void NJS::RValue::Store(ValuePtr) const
{
}

void NJS::RValue::StoreNoError(ValuePtr) const
{
}

NJS::RValue::RValue(Builder &builder, TypePtr type, llvm::Value *value)
    : Value(builder, std::move(type)),
      m_Value(value)
{
}
