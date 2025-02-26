#include <newjs/builder.hpp>
#include <newjs/error.hpp>
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

NJS::TypePtr NJS::ArrayType::GetElement(const SourceLocation &where) const
{
    return m_ElementType;
}

NJS::TypePtr NJS::ArrayType::GetElement(const SourceLocation &where, const unsigned index) const
{
    if (index >= m_Count)
        Error(where, "array index out of bounds: {} !E [0,{})", index, m_Count);
    return m_ElementType;
}

unsigned NJS::ArrayType::GetElementCount(const SourceLocation &) const
{
    return m_Count;
}

bool NJS::ArrayType::TypeInfo(
    const SourceLocation &where,
    Builder &builder,
    std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_ARRAY));
    arguments.emplace_back(builder.GetBuilder().getInt32(m_Count));
    return m_ElementType->TypeInfo(where, builder, arguments);
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

llvm::Type *NJS::ArrayType::GenLLVM(const SourceLocation &where, const Builder &builder) const
{
    return llvm::ArrayType::get(m_ElementType->GetLLVM(where, builder), m_Count);
}
