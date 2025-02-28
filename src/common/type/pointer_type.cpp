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

bool NJS::PointerType::IsConst() const
{
    return m_IsConst;
}

NJS::TypePtr NJS::PointerType::GetElement() const
{
    return m_ElementType;
}

bool NJS::PointerType::TypeInfo(
    Builder &builder,
    std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_POINTER));
    m_ElementType->TypeInfo(builder, arguments);
    return false;
}

NJS::PointerType::PointerType(TypeContext &type_context, std::string string, TypePtr element_type, const bool is_const)
    : Type(type_context, std::move(string)),
      m_ElementType(std::move(element_type)),
      m_IsConst(is_const)
{
}

llvm::Type *NJS::PointerType::GenLLVM(const Builder &builder) const
{
    return builder.GetBuilder().getPtrTy();
}
