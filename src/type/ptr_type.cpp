#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::PtrType::GenString(const TypePtr& element)
{
    return element->GetString() + "[]";
}

bool NJS::PtrType::IsPrimitive() const
{
    return true;
}

bool NJS::PtrType::IsPtr() const
{
    return true;
}

NJS::TypePtr NJS::PtrType::GetElement() const
{
    return m_Element;
}

void NJS::PtrType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_POINTER));
    m_Element->TypeInfo(builder, args);
}

NJS::PtrType::PtrType(TypeContext& ctx, std::string string, TypePtr element)
    : Type(ctx, std::move(string)), m_Element(std::move(element))
{
}

llvm::Type* NJS::PtrType::GenLLVM(const Builder& builder) const
{
    return llvm::PointerType::get(m_Element->GetLLVM(builder), 0);
}

unsigned NJS::PtrType::GenSize() const
{
    return 8;
}
