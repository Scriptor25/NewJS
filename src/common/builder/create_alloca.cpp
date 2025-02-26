#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

llvm::Value *NJS::Builder::CreateAlloca(llvm::Type *type, const unsigned count) const
{
    const auto bkp = GetBuilder().GetInsertBlock();
    GetBuilder().SetInsertPointPastAllocas(bkp->getParent());
    const auto ptr = GetBuilder().CreateAlloca(type, count ? GetBuilder().getInt64(count) : nullptr);
    GetBuilder().SetInsertPoint(bkp);
    return ptr;
}

NJS::ValuePtr NJS::Builder::CreateAlloca(
    const SourceLocation &where,
    const TypePtr &type,
    const bool is_const,
    const unsigned count)
{
    const auto ptr = CreateAlloca(type->GetLLVM(where, *this), count);
    return LValue::Create(*this, type, ptr, is_const);
}
