#include <NJS/Builder.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::TupleType::GenString(const std::vector<TypePtr>& element_types)
{
    std::string str;

    str += "[ ";
    bool first = true;
    for (const auto& type : element_types)
    {
        if (first) first = false;
        else str += ", ";
        str += type->String;
    }
    return str + " ]";
}

NJS::TupleType::TupleType(TypeContext& ctx, std::vector<TypePtr> element_types)
    : Type(ctx, GenString(element_types)), ElementTypes(std::move(element_types))
{
}

bool NJS::TupleType::IsTuple() const
{
    return true;
}

NJS::TypePtr NJS::TupleType::Element(const size_t i) const
{
    return ElementTypes[i];
}

size_t NJS::TupleType::NumElements() const
{
    return ElementTypes.size();
}

void NJS::TupleType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_TUPLE));
    args.push_back(builder.GetBuilder().getInt64(ElementTypes.size()));
    for (const auto& element : ElementTypes)
        element->TypeInfo(builder, args);
}

size_t NJS::TupleType::Bytes() const
{
    size_t bytes = 0;
    for (const auto& element : ElementTypes)
        bytes += element->Bytes();
    return bytes;
}

llvm::Type* NJS::TupleType::GenLLVM(Builder& builder) const
{
    std::vector<llvm::Type*> elements;
    for (const auto& element : ElementTypes)
        elements.push_back(element->GenLLVM(builder));
    return llvm::StructType::get(builder.GetContext(), elements);
}
