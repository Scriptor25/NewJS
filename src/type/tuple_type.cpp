#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>

#include "NJS/Std.hpp"
#include "NJS/TypeContext.hpp"

std::string NJS::TupleType::GenString(const std::vector<TypePtr> &elements)
{
    std::string dst = "[ ";
    for (unsigned i = 0; i < elements.size(); ++i)
    {
        if (i > 0)
            dst += ", ";
        dst += elements[i]->GetString();
    }
    return dst += " ]";
}

bool NJS::TupleType::IsTuple() const
{
    return true;
}

NJS::TypePtr NJS::TupleType::GetElement(const unsigned i) const
{
    return m_Elements[i];
}

void NJS::TupleType::TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_TUPLE));
    args.push_back(builder.GetBuilder().getInt32(m_Elements.size()));
    for (const auto &element: m_Elements)
        element->TypeInfo(where, builder, args);
}

NJS::TupleType::TupleType(
    TypeContext &ctx,
    std::string string,
    std::vector<TypePtr> elements)
    : Type(ctx, std::move(string)),
      m_Elements(std::move(elements))
{
}

llvm::Type *NJS::TupleType::GenLLVM(const SourceLocation &where, const Builder &builder) const
{
    std::vector<llvm::Type *> types;
    for (const auto &element: m_Elements)
        types.push_back(element->GetLLVM(where, builder));
    return llvm::StructType::get(builder.GetContext(), types, true);
}

unsigned NJS::TupleType::GenSize() const
{
    unsigned size = 0;
    for (const auto &element: m_Elements)
        size += element->GetSize();
    return size;
}
