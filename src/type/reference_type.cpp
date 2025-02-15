#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>

std::string NJS::ReferenceType::GenString(const TypePtr &element_type)
{
    return element_type->GetString() + '&';
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

void NJS::ReferenceType::TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const
{
    m_ElementType->TypeInfo(where, builder, args);
}

NJS::ReferenceType::ReferenceType(
    TypeContext &type_context,
    std::string string,
    TypePtr element_type)
    : Type(type_context, std::move(string)),
      m_ElementType(std::move(element_type))
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
