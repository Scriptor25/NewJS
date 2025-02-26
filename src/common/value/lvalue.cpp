#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::LValue::Create(Builder &builder, const TypePtr &type, llvm::Value *ptr, const bool is_const)
{
    return std::shared_ptr<LValue>(new LValue(builder, type, ptr, is_const));
}

bool NJS::LValue::IsLValue() const
{
    return true;
}

bool NJS::LValue::IsConst() const
{
    return m_IsConst;
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
    if (m_IsConst)
        Error(where, "invalid store: lvalue is marked constant");

    if (value->getType() != GetType()->GetLLVM(where, GetBuilder()))
        Error(where, "invalid store: type mismatch, <llvm type> != {}", GetType());

    GetBuilder().GetBuilder().CreateStore(value, m_Ptr);
}

void NJS::LValue::Store(const SourceLocation &where, ValuePtr value) const
{
    if (m_IsConst)
        Error(where, "invalid store: lvalue is marked constant");

    value = GetBuilder().CreateCast(where, value, GetType());
    GetBuilder().GetBuilder().CreateStore(value->Load(where), m_Ptr);
}

void NJS::LValue::StoreForce(const SourceLocation &where, ValuePtr value) const
{
    value = GetBuilder().CreateCast(where, value, GetType());
    GetBuilder().GetBuilder().CreateStore(value->Load(where), m_Ptr);
}

NJS::LValue::LValue(Builder &builder, TypePtr type, llvm::Value *ptr, const bool is_const)
    : Value(builder, std::move(type)),
      m_Ptr(ptr),
      m_IsConst(is_const)
{
}
