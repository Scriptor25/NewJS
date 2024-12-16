#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

std::string NJS::NoType::GenString()
{
    return {};
}

NJS::TypePtr NJS::NoType::GetResult() const
{
    return m_Ctx.GetNoType();
}

void NJS::NoType::TypeInfo(Builder&, std::vector<llvm::Value*>&) const
{
    Error("the no-type does not provide type information");
}

NJS::NoType::NoType(TypeContext& ctx, std::string string)
    : Type(ctx, std::move(string))
{
}

llvm::Type* NJS::NoType::GenLLVM(const Builder&) const
{
    Error("the no-type does not have a llvm representation");
}

unsigned NJS::NoType::GenSize() const
{
    return 0;
}
