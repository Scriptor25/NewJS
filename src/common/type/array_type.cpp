#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>

std::string NJS::ArrayType::GenString(const TypePtr &element_type, const unsigned count)
{
    return element_type->GetString() + '[' + std::to_string(count) + ']';
}

bool NJS::ArrayType::IsArray() const
{
    return true;
}

NJS::TypePtr NJS::ArrayType::GetElement() const
{
    return m_ElementType;
}

unsigned NJS::ArrayType::GetElementCount() const
{
    return m_Count;
}

bool NJS::ArrayType::TypeInfo(
    Builder &builder,
    std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_ARRAY));
    arguments.emplace_back(builder.GetBuilder().getInt32(m_Count));
    return m_ElementType->TypeInfo(builder, arguments);
}

NJS::ArrayType::ArrayType(
    TypeContext &type_context,
    std::string string,
    TypePtr element_type,
    const unsigned count)
    : Type(type_context, std::move(string)),
      m_ElementType(std::move(element_type)),
      m_Count(count)
{
}

llvm::Type *NJS::ArrayType::GenLLVM(const Builder &builder) const
{
    return llvm::ArrayType::get(m_ElementType->GetLLVM(builder), m_Count);
}
