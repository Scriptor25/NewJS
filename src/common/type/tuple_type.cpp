#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>

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

unsigned NJS::TupleType::GenHash(const std::vector<TypePtr> &element_types)
{
    unsigned hash = 0x07;
    for (auto &element: element_types)
        hash = CombineHashes(hash, element->GetHash());
    return hash;
}

bool NJS::TupleType::IsTuple() const
{
    return true;
}

NJS::TypePtr NJS::TupleType::GetElement(const unsigned index) const
{
    if (index >= m_ElementTypes.size())
        Error("cannot get element at {} of tuple type {}", index, m_String);
    return m_ElementTypes[index];
}

unsigned NJS::TupleType::GetElementCount() const
{
    return m_ElementTypes.size();
}

bool NJS::TupleType::TypeInfo(
    Builder &builder,
    std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_TUPLE));
    arguments.emplace_back(builder.GetBuilder().getInt32(m_ElementTypes.size()));
    auto any_incomplete = false;
    for (const auto &element: m_ElementTypes)
        any_incomplete |= element->TypeInfo(builder, arguments);
    return any_incomplete;
}

std::ostream &NJS::TupleType::Print(std::ostream &stream) const
{
    stream << "[ ";
    for (unsigned i = 0; i < m_ElementTypes.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        m_ElementTypes[i]->Print(stream);
    }
    return stream << " ]";
}

NJS::TupleType::TupleType(
    TypeContext &type_context,
    const unsigned hash,
    std::string string,
    std::vector<TypePtr> element_types)
    : Type(type_context, hash, std::move(string)),
      m_ElementTypes(std::move(element_types))
{
}

llvm::Type *NJS::TupleType::GenLLVM(const Builder &builder) const
{
    std::vector<llvm::Type *> types;
    for (const auto &element_type: m_ElementTypes)
        types.emplace_back(element_type->GetLLVM(builder));
    return llvm::StructType::get(builder.GetContext(), types, true);
}
