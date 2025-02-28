#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::Builder::CreateGlobal(
    const std::string &name,
    const TypePtr &type,
    const bool is_const,
    const bool initialize,
    llvm::Constant *initializer)
{
    const auto ty = type->GetLLVM(*this);
    const auto global = new llvm::GlobalVariable(
        GetModule(),
        ty,
        is_const,
        llvm::GlobalValue::ExternalLinkage,
        initializer ? initializer : initialize ? llvm::Constant::getNullValue(ty) : nullptr,
        name);
    return LValue::Create(*this, type, global, is_const);
}
