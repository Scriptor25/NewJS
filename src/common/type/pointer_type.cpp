#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>

std::string NJS::PointerType::GenString(const TypePtr &element_type, const bool is_const)
{
    return element_type->GetString() + '[' + (is_const ? "const" : "") + ']';
}

bool NJS::PointerType::IsPrimitive() const
{
    return true;
}

bool NJS::PointerType::IsPointer() const
{
    return true;
}

bool NJS::PointerType::IsConst(const SourceLocation &) const
{
    return m_IsConst;
}

NJS::TypePtr NJS::PointerType::GetElement(const SourceLocation &) const
{
    return m_ElementType;
}

NJS::TypePtr NJS::PointerType::GetElement(const SourceLocation &, unsigned) const
{
    return m_ElementType;
}

unsigned NJS::PointerType::GetElementCount(const SourceLocation &) const
{
    return 1;
}

bool NJS::PointerType::TypeInfo(
    const SourceLocation &where,
    Builder &builder,
    std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_POINTER));
    m_ElementType->TypeInfo(where, builder, arguments);
    return false;
}

NJS::PointerType::PointerType(TypeContext &type_context, std::string string, TypePtr element_type, const bool is_const)
    : Type(type_context, std::move(string)),
      m_ElementType(std::move(element_type)),
      m_IsConst(is_const)
{
}

llvm::Type *NJS::PointerType::GenLLVM(const SourceLocation &, const Builder &builder) const
{
    return llvm::PointerType::get(builder.GetContext(), 0u);
}
