#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::TupleType::GenString(const std::vector<TypePtr> &element_types)
{
    std::string dst = "[ ";
    for (unsigned i = 0; i < element_types.size(); ++i)
    {
        if (i > 0)
            dst += ", ";
        dst += element_types[i]->GetString();
    }
    return dst += " ]";
}

bool NJS::TupleType::IsTuple() const
{
    return true;
}

NJS::TypePtr NJS::TupleType::GetElement(const unsigned index) const
{
    return m_ElementTypes[index];
}

void NJS::TupleType::TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_TUPLE));
    args.push_back(builder.GetBuilder().getInt32(m_ElementTypes.size()));
    for (const auto &element: m_ElementTypes)
        element->TypeInfo(where, builder, args);
}

NJS::TupleType::TupleType(
    TypeContext &type_context,
    std::string string,
    std::vector<TypePtr> element_types)
    : Type(type_context, std::move(string)),
      m_ElementTypes(std::move(element_types))
{
}

llvm::Type *NJS::TupleType::GenLLVM(const SourceLocation &where, const Builder &builder) const
{
    std::vector<llvm::Type *> types;
    for (const auto &element: m_ElementTypes)
        types.push_back(element->GetLLVM(where, builder));
    return llvm::StructType::get(builder.GetContext(), types, true);
}

unsigned NJS::TupleType::GenSize() const
{
    unsigned size = 0;
    for (const auto &element: m_ElementTypes)
        size += element->GetSize();
    return size;
}
