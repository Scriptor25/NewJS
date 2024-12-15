#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::VoidType::GenString()
{
    return "void";
}

bool NJS::VoidType::IsPrimitive() const
{
    return true;
}

bool NJS::VoidType::IsVoid() const
{
    return true;
}

void NJS::VoidType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_VOID));
}

NJS::VoidType::VoidType(TypeContext& ctx, std::string string)
    : Type(ctx, std::move(string))
{
}

llvm::Type* NJS::VoidType::GenLLVM(const Builder& builder) const
{
    return builder.GetBuilder().getVoidTy();
}

unsigned NJS::VoidType::GenSize() const
{
    return 0;
}
