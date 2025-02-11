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

void NJS::VoidType::TypeInfo(const SourceLocation &, Builder &builder, std::vector<llvm::Value *> &args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_VOID));
}

NJS::VoidType::VoidType(TypeContext &type_context, std::string_view string)
    : Type(type_context, std::move(string))
{
}

llvm::Type *NJS::VoidType::GenLLVM(const SourceLocation &, const Builder &builder) const
{
    return builder.GetBuilder().getVoidTy();
}

unsigned NJS::VoidType::GenSize() const
{
    return 0;
}
