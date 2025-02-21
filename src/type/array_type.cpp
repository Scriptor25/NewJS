#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

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

void NJS::ArrayType::TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_ARRAY));
    args.push_back(builder.GetBuilder().getInt32(m_Count));
    m_ElementType->TypeInfo(where, builder, args);
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

unsigned NJS::ArrayType::GenSize() const
{
    return m_ElementType->GetSize() * m_Count;
}
