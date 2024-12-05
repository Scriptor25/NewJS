#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::LValue::Create(Builder& builder, const TypePtr& type, llvm::Value* ptr)
{
    return std::shared_ptr<LValue>(new LValue(builder, type, ptr));
}

bool NJS::LValue::IsL() const
{
    return true;
}

llvm::Value* NJS::LValue::GetPtr() const
{
    return m_Ptr;
}

llvm::Value* NJS::LValue::Load() const
{
    return GetBuilder().LLVMBuilder().CreateLoad(GetType()->GenLLVM(GetBuilder()), m_Ptr);
}

void NJS::LValue::Store(llvm::Value* value) const
{
    if (value->getType() != GetType()->GenLLVM(GetBuilder()))
        Error("invalid store: type mismatch, <llvm type> != {}", GetType());
    GetBuilder().LLVMBuilder().CreateStore(value, m_Ptr);
}

void NJS::LValue::Store(const ValuePtr value) const
{
    if (value->GetType() != GetType())
        Error("invalid store: type mismatch, {} != {}", value->GetType(), GetType());
    GetBuilder().LLVMBuilder().CreateStore(value->Load(), m_Ptr);
}

NJS::LValue::LValue(Builder& builder, TypePtr type, llvm::Value* ptr)
    : Value(builder, std::move(type)), m_Ptr(ptr)
{
}
