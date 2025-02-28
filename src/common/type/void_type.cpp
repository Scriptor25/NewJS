#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>

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

bool NJS::VoidType::TypeInfo(Builder &builder, std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_VOID));
    return false;
}

NJS::VoidType::VoidType(TypeContext &type_context, std::string string)
    : Type(type_context, std::move(string))
{
}

llvm::Type *NJS::VoidType::GenLLVM(const Builder &builder) const
{
    return builder.GetBuilder().getVoidTy();
}
