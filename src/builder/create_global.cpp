#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateGlobal(
    const SourceLocation &where,
    const std::string &name,
    const TypePtr &type,
    const bool is_const,
    const bool initialize,
    llvm::Constant *initializer)
{
    const auto ty = type->GetLLVM(where, *this);
    const auto global = new llvm::GlobalVariable(
        GetModule(),
        ty,
        is_const,
        llvm::GlobalValue::ExternalLinkage,
        initializer ? initializer : initialize ? llvm::Constant::getNullValue(ty) : nullptr,
        name);
    return LValue::Create(*this, type, global, is_const);
}
