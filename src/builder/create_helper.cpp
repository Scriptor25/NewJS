#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

llvm::Value* NJS::Builder::CreateAlloca(llvm::Type* type, const unsigned size) const
{
    const auto bkp = GetBuilder().GetInsertBlock();
    GetBuilder().SetInsertPointPastAllocas(bkp->getParent());
    const auto ptr = GetBuilder().CreateAlloca(type, size ? GetBuilder().getInt64(size) : nullptr);
    GetBuilder().SetInsertPoint(bkp);
    return ptr;
}

NJS::ValuePtr NJS::Builder::CreateAlloca(const TypePtr& type, const unsigned size)
{
    const auto ptr = CreateAlloca(type->GetLLVM(*this), size);
    return LValue::Create(*this, type, ptr);
}

NJS::ValuePtr NJS::Builder::CreateGlobal(const std::string& name, const TypePtr& type, const bool init)
{
    const auto ty = type->GetLLVM(*this);
    const auto global = new llvm::GlobalVariable(
        GetModule(),
        ty,
        false,
        llvm::GlobalValue::ExternalLinkage,
        init ? llvm::Constant::getNullValue(ty) : nullptr,
        name);
    return LValue::Create(*this, type, global);
}
