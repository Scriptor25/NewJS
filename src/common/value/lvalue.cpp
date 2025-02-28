#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::LValue::Create(Builder &builder, const TypePtr &type, llvm::Value *pointer, const bool is_const)
{
    return std::shared_ptr<LValue>(new LValue(builder, type, pointer, is_const));
}

bool NJS::LValue::IsLValue() const
{
    return true;
}

bool NJS::LValue::IsConst() const
{
    return m_IsConst;
}

llvm::Value *NJS::LValue::GetPointer() const
{
    return m_Pointer;
}

llvm::Value *NJS::LValue::Load() const
{
    return GetBuilder().GetBuilder().CreateLoad(GetType()->GetLLVM(GetBuilder()), m_Pointer);
}

void NJS::LValue::Store(llvm::Value *value) const
{
    if (m_IsConst)
        return;

    if (value->getType() != GetType()->GetLLVM(GetBuilder()))
        return;

    GetBuilder().GetBuilder().CreateStore(value, m_Pointer);
}

void NJS::LValue::Store(ValuePtr value) const
{
    if (m_IsConst)
        return;

    value = GetBuilder().CreateCast(value, GetType());
    GetBuilder().GetBuilder().CreateStore(value->Load(), m_Pointer);
}

void NJS::LValue::StoreNoError(ValuePtr value) const
{
    value = GetBuilder().CreateCast(value, GetType());
    GetBuilder().GetBuilder().CreateStore(value->Load(), m_Pointer);
}

NJS::LValue::LValue(Builder &builder, TypePtr type, llvm::Value *pointer, const bool is_const)
    : Value(builder, std::move(type)),
      m_Pointer(pointer),
      m_IsConst(is_const)
{
}
