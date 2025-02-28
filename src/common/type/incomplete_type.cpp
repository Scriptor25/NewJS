#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>

std::string NJS::IncompleteType::GenString(const std::string &name)
{
    return name;
}

bool NJS::IncompleteType::IsIncomplete() const
{
    return true;
}

bool NJS::IncompleteType::TypeInfo(
    Builder &builder,
    std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_INCOMPLETE));
    arguments.emplace_back(StringExpression::GetString(builder, m_Name));
    return true;
}

NJS::IncompleteType::IncompleteType(TypeContext &type_context, std::string string, std::string name)
    : Type(type_context, std::move(string)),
      m_Name(std::move(name))
{
}

llvm::Type *NJS::IncompleteType::GenLLVM(const Builder &) const
{
    return nullptr;
}
