#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>

std::string NJS::RefType::GenString(const TypePtr &element)
{
    return element->GetString() + '&';
}

bool NJS::RefType::IsPrimitive() const
{
    return true;
}

bool NJS::RefType::IsRef() const
{
    return true;
}

NJS::TypePtr NJS::RefType::GetElement() const
{
    return m_Element;
}

void NJS::RefType::TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const
{
    m_Element->TypeInfo(where, builder, args);
}

NJS::RefType::RefType(TypeContext &ctx, std::string string, TypePtr element)
    : Type(ctx, std::move(string)),
      m_Element(std::move(element))
{
}

llvm::Type *NJS::RefType::GenLLVM(const SourceLocation &, const Builder &builder) const
{
    return llvm::PointerType::get(builder.GetContext(), 0u);
}

unsigned NJS::RefType::GenSize() const
{
    return 8;
}
