#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateGlobal(
    const SourceLocation &where,
    const std::string &name,
    const TypePtr &type,
    const bool initialize,
    const bool is_mutable)
{
    const auto ty = type->GetLLVM(where, *this);
    const auto global = new llvm::GlobalVariable(
        GetModule(),
        ty,
        false,
        llvm::GlobalValue::ExternalLinkage,
        initialize ? llvm::Constant::getNullValue(ty) : nullptr,
        name);
    return LValue::Create(*this, type, global, is_mutable);
}
