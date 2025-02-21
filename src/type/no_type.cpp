#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

std::string NJS::NoType::GenString(const std::string &name)
{
    return name;
}

bool NJS::NoType::IsIncomplete() const
{
    return true;
}

void NJS::NoType::TypeInfo(const SourceLocation &where, Builder &, std::vector<llvm::Value *> &) const
{
    Error(where, "the incomplete type '{}' does not provide type information", m_Name);
}

NJS::NoType::NoType(TypeContext &type_context, std::string string, std::string name)
    : Type(type_context, std::move(string)),
      m_Name(std::move(name))
{
}

llvm::Type *NJS::NoType::GenLLVM(const SourceLocation &where, const Builder &) const
{
    Error(where, "the incomplete type '{}' does not have a llvm representation", m_Name);
}

unsigned NJS::NoType::GenSize() const
{
    return 0;
}
