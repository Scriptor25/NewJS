#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::LValue::Create(Builder &builder, const TypePtr &type, llvm::Value *ptr)
{
    return std::shared_ptr<LValue>(new LValue(builder, type, ptr));
}

bool NJS::LValue::IsL() const
{
    return true;
}

llvm::Value *NJS::LValue::GetPtr(const SourceLocation &) const
{
    return m_Ptr;
}

llvm::Value *NJS::LValue::Load(const SourceLocation &where) const
{
    return GetBuilder().GetBuilder().CreateLoad(GetType()->GetLLVM(where, GetBuilder()), m_Ptr);
}

void NJS::LValue::Store(const SourceLocation &where, llvm::Value *value) const
{
    if (value->getType() != GetType()->GetLLVM(where, GetBuilder()))
        Error(where, "invalid store: type mismatch, <llvm type> != {}", GetType());

    GetBuilder().GetBuilder().CreateStore(value, m_Ptr);
}

void NJS::LValue::Store(const SourceLocation &where, ValuePtr value) const
{
    value = GetBuilder().CreateCast(where, value, GetType());
    GetBuilder().GetBuilder().CreateStore(value->Load(where), m_Ptr);
}

NJS::LValue::LValue(Builder &builder, TypePtr type, llvm::Value *ptr)
    : Value(builder, std::move(type)),
      m_Ptr(ptr)
{
}
