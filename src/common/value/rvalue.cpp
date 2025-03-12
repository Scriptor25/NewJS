#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::RValue::Create(const Builder &builder, const TypePtr &type, llvm::Value *value)
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
    Error("cannot get pointer to rvalue");
}

llvm::Value *NJS::RValue::Load() const
{
    return m_Value;
}

void NJS::RValue::Store(llvm::Value *) const
{
    Error("cannot store to rvalue");
}

void NJS::RValue::Store(ValuePtr) const
{
    Error("cannot store to rvalue");
}

void NJS::RValue::StoreNoError(ValuePtr) const
{
    Error("cannot store to rvalue");
}

NJS::RValue::RValue(const Builder &builder, TypePtr type, llvm::Value *value)
    : Value(builder, std::move(type)),
      m_Value(value)
{
}
