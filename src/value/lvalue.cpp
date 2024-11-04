#include <NJS/Builder.hpp>
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

llvm::Value* NJS::LValue::Load()
{
    return GetBuilder().LLVMBuilder().CreateLoad(GetLLVMType(), m_Ptr);
}

void NJS::LValue::Store(llvm::Value* value)
{
    GetBuilder().LLVMBuilder().CreateStore(value, m_Ptr);
}

NJS::LValue::LValue(Builder& builder, TypePtr type, llvm::Value* ptr)
    : Value(builder, std::move(type)), m_Ptr(ptr)
{
}
