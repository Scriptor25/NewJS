#include <NJS/Error.hpp>
#include <NJS/Type.hpp>

std::string NJS::NoType::GenString()
{
    return {};
}

NJS::NoType::NoType(TypeContext& ctx)
    : Type(ctx, GenString())
{
}

void NJS::NoType::TypeInfo(Builder&, std::vector<llvm::Value*>&) const
{
    Error("type has no info");
}

size_t NJS::NoType::Bytes() const
{
    Error("type has no size");
}

llvm::Type* NJS::NoType::GenLLVM(Builder&) const
{
    Error("type has no llvm representation");
}
