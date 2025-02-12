#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

std::string NJS::NoType::GenString(const std::string_view &name)
{
    return std::string(name);
}

NJS::TypePtr NJS::NoType::GetResultType() const
{
    return m_TypeContext.GetNoType(m_Name + "::RESULT");
}

void NJS::NoType::TypeInfo(const SourceLocation &, Builder &, std::vector<llvm::Value *> &) const
{
    Error("the no-type does not provide type information");
}

NJS::NoType::NoType(TypeContext &type_context, std::string_view string, std::string_view name)
    : Type(type_context, std::move(string)),
      m_Name(std::move(name))
{
}

llvm::Type *NJS::NoType::GenLLVM(const SourceLocation &where, const Builder &) const
{
    Error(where, "the no-type does not have a llvm representation");
}

unsigned NJS::NoType::GenSize() const
{
    return 0;
}
