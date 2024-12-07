#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

llvm::Value* NJS::Builder::CreateMalloc(const size_t size) const
{
    return CreateMalloc(GetBuilder().getInt64(size));
}

llvm::Value* NJS::Builder::CreateMalloc(llvm::Value* size) const
{
    const auto type = llvm::FunctionType::get(
        GetBuilder().getPtrTy(),
        {GetBuilder().getInt64Ty()},
        false);
    const auto callee = GetModule().getOrInsertFunction("malloc", type);
    return GetBuilder().CreateCall(callee, {size});
}

llvm::Value* NJS::Builder::CreateRealloc(llvm::Value* ptr, llvm::Value* size) const
{
    const auto type = llvm::FunctionType::get(
        GetBuilder().getPtrTy(),
        {GetBuilder().getPtrTy(), GetBuilder().getInt64Ty()},
        false);
    const auto callee = GetModule().getOrInsertFunction("realloc", type);
    return GetBuilder().CreateCall(callee, {ptr, size});
}

llvm::Value* NJS::Builder::CreateMemcpy(llvm::Value* dst, llvm::Value* src, llvm::Value* count) const
{
    const auto type = llvm::FunctionType::get(
        GetBuilder().getPtrTy(),
        {GetBuilder().getPtrTy(), GetBuilder().getPtrTy(), GetBuilder().getInt64Ty()},
        false);
    const auto callee = GetModule().getOrInsertFunction("memcpy", type);
    return GetBuilder().CreateCall(callee, {dst, src, count});
}

llvm::Value* NJS::Builder::CreateStrlen(llvm::Value* str) const
{
    const auto type = llvm::FunctionType::get(
        GetBuilder().getInt64Ty(),
        {GetBuilder().getPtrTy()},
        false);
    const auto callee = GetModule().getOrInsertFunction("strlen", type);
    return GetBuilder().CreateCall(callee, {str});
}

llvm::Value* NJS::Builder::CreateAlloca(llvm::Type* type, const size_t size) const
{
    const auto bkp = GetBuilder().GetInsertBlock();
    GetBuilder().SetInsertPointPastAllocas(bkp->getParent());
    const auto ptr = GetBuilder().CreateAlloca(type, size ? GetBuilder().getInt64(size) : nullptr);
    GetBuilder().SetInsertPoint(bkp);
    return ptr;
}

NJS::ValuePtr NJS::Builder::CreateAlloca(const TypePtr& type, const size_t size)
{
    const auto ptr = CreateAlloca(type->GenLLVM(*this), size);
    return LValue::Create(*this, type, ptr);
}

NJS::ValuePtr NJS::Builder::CreateGlobal(const std::string& name, const TypePtr& type, const bool init)
{
    const auto ty = type->GenLLVM(*this);
    const auto global = new llvm::GlobalVariable(
        GetModule(),
        ty,
        false,
        llvm::GlobalValue::ExternalLinkage,
        init ? llvm::Constant::getNullValue(ty) : nullptr,
        name);
    return LValue::Create(*this, type, global);
}
