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

NJS::TypePtr NJS::TupleType::GetElement(const SourceLocation &where, const unsigned index) const
{
    if (index >= m_ElementTypes.size())
        Error(where, "tuple index out of bounds: {} !E [0,{})", index, m_ElementTypes.size());
    return m_ElementTypes[index];
}

unsigned NJS::TupleType::GetElementCount(const SourceLocation &) const
{
    return m_ElementTypes.size();
}

bool NJS::TupleType::TypeInfo(
    const SourceLocation &where,
    Builder &builder,
    std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_TUPLE));
    arguments.emplace_back(builder.GetBuilder().getInt32(m_ElementTypes.size()));
    auto any_incomplete = false;
    for (const auto &element: m_ElementTypes)
        any_incomplete |= element->TypeInfo(where, builder, arguments);
    return any_incomplete;
}

static unsigned tuple_count = 0;

NJS::TupleType::TupleType(
    TypeContext &type_context,
    std::string string,
    std::vector<TypePtr> element_types)
    : Type(type_context, std::move(string)),
      m_ElementTypes(std::move(element_types)),
      m_Index(tuple_count++)
{
}

llvm::Type *NJS::TupleType::GenLLVM(const SourceLocation &where, const Builder &builder) const
{
    const auto tuple_name = "tuple." + std::to_string(m_Index);
    if (const auto tuple_type = llvm::StructType::getTypeByName(builder.GetContext(), tuple_name))
        return tuple_type;

    std::vector<llvm::Type *> types;
    for (const auto &element: m_ElementTypes)
        types.emplace_back(element->GetLLVM(where, builder));

    return llvm::StructType::create(builder.GetContext(), types, tuple_name, true);
}
