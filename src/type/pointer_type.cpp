#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::PointerType::GenString(const TypePtr& element)
{
    return element->GetString() + "[]";
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
    return m_Element;
}

void NJS::PointerType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_POINTER));
    m_Element->TypeInfo(builder, args);
}

NJS::PointerType::PointerType(TypeContext& ctx, std::string string, TypePtr element)
    : Type(ctx, std::move(string)), m_Element(std::move(element))
{
}

llvm::Type* NJS::PointerType::GenLLVM(const Builder& builder) const
{
    return llvm::PointerType::get(m_Element->GetLLVM(builder), 0);
}

unsigned NJS::PointerType::GenSize() const
{
    return 8;
}
