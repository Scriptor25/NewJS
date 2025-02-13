#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>

std::string NJS::ReferenceType::GenString(const TypePtr &element_type, const bool is_mutable)
{
    return element_type->GetString() + (is_mutable ? "" : " const") + '&';
}

bool NJS::ReferenceType::IsPrimitive() const
{
    return true;
}

bool NJS::ReferenceType::IsReference() const
{
    return true;
}

NJS::TypePtr NJS::ReferenceType::GetElement() const
{
    return m_ElementType;
}

bool NJS::ReferenceType::IsMutable() const
{
    return m_IsMutable;
}

void NJS::ReferenceType::TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const
{
    m_ElementType->TypeInfo(where, builder, args);
}

NJS::ReferenceType::ReferenceType(
    TypeContext &type_context,
    std::string_view string,
    TypePtr element_type,
    const bool is_mutable)
    : Type(type_context, std::move(string)),
      m_ElementType(std::move(element_type)),
      m_IsMutable(is_mutable)
{
}

llvm::Type *NJS::ReferenceType::GenLLVM(const SourceLocation &, const Builder &builder) const
{
    return llvm::PointerType::get(builder.GetContext(), 0u);
}

unsigned NJS::ReferenceType::GenSize() const
{
    return 8;
}
