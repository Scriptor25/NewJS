#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::Builder::CreateGlobal(
    const std::string &name,
    const TypePtr &type,
    const bool is_const,
    const bool initialize,
    llvm::Constant *initializer) const
{
    const auto value_type = type->GetLLVM(*this);
    const auto global = new llvm::GlobalVariable(
        GetModule(),
        value_type,
        false,
        llvm::GlobalValue::ExternalLinkage,
        initializer
            ? initializer
            : initialize
                  ? llvm::Constant::getNullValue(value_type)
                  : nullptr,
        name);
    return LValue::Create(*this, type, global, is_const);
}
