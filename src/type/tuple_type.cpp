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

NJS::TupleType::TupleType(std::vector<TypePtr> element_types)
    : Type(GenString(element_types)), ElementTypes(std::move(element_types))
{
}

size_t NJS::TupleType::Size() const
{
    size_t size = 0;
    for (const auto& type : ElementTypes)
        size += type->Size();
    return size;
}

NJS::TypePtr NJS::TupleType::Element(const size_t i)
{
    return ElementTypes[i];
}

void NJS::TupleType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.LLVMBuilder().getInt32(ID_TUPLE));
    args.push_back(builder.LLVMBuilder().getInt64(ElementTypes.size()));
    for (const auto& element : ElementTypes)
        element->TypeInfo(builder, args);
}

llvm::Type* NJS::TupleType::GenLLVM(Builder& builder) const
{
    std::vector<llvm::Type*> elements;
    for (const auto& element : ElementTypes)
        elements.push_back(element->GenLLVM(builder));
    return llvm::StructType::get(builder.LLVMContext(), elements);
}
