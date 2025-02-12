#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::PointerType::GenString(const TypePtr &element_type)
{
    return element_type->GetString() + "[]";
}

bool NJS::PointerType::IsPrimitive() const
{
    return true;
}

bool NJS::PointerType::IsPointer() const
{
    return true;
}

NJS::TypePtr NJS::PointerType::GetElement() const
{
    return m_ElementType;
}

void NJS::PointerType::TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_POINTER));
    m_ElementType->TypeInfo(where, builder, args);
}

NJS::PointerType::PointerType(TypeContext &type_context, std::string_view string, TypePtr element_type)
    : Type(type_context, std::move(string)),
      m_ElementType(std::move(element_type))
{
}

llvm::Type *NJS::PointerType::GenLLVM(const SourceLocation &, const Builder &builder) const
{
    return llvm::PointerType::get(builder.GetContext(), 0u);
}

unsigned NJS::PointerType::GenSize() const
{
    return 8;
}
