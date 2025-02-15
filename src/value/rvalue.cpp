#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::RValue::Create(Builder &builder, const TypePtr &type, llvm::Value *value)
{
    return std::shared_ptr<RValue>(new RValue(builder, type, value));
}

bool NJS::RValue::IsLValue() const
{
    return false;
}

llvm::Value *NJS::RValue::GetPtr(const SourceLocation &where) const
{
    Error(where, "cannot get pointer to rvalue");
}

llvm::Value *NJS::RValue::Load(const SourceLocation &) const
{
    return m_Value;
}

void NJS::RValue::Store(const SourceLocation &where, llvm::Value *, bool force) const
{
    Error(where, "cannot assign to rvalue");
}

void NJS::RValue::Store(const SourceLocation &where, ValuePtr) const
{
    Error(where, "cannot assign to rvalue");
}

NJS::RValue::RValue(Builder &builder, TypePtr type, llvm::Value *value)
    : Value(builder, std::move(type)),
      m_Value(value)
{
}
