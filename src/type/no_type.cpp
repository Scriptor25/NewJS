#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

std::string NJS::NoType::GenString(const std::string& name)
{
    return name;
}

NJS::TypePtr NJS::NoType::GetResult() const
{
    return m_Ctx.GetNoType(m_Name + "::RESULT");
}

void NJS::NoType::TypeInfo(const SourceLocation&, Builder&, std::vector<llvm::Value*>&) const
{
    Error("the no-type does not provide type information");
}

NJS::NoType::NoType(TypeContext& ctx, std::string string, std::string name)
    : Type(ctx, std::move(string)), m_Name(std::move(name))
{
}

llvm::Type* NJS::NoType::GenLLVM(const SourceLocation& where, const Builder&) const
{
    Error(where, "the no-type does not have a llvm representation");
}

unsigned NJS::NoType::GenSize() const
{
    return 0;
}
