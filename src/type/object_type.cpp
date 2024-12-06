#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::ObjectType::GenString(const std::map<std::string, TypePtr>& element_types)
{
    std::string str;

    str += "{ ";
    bool first = true;
    for (const auto& [name, type] : element_types)
    {
        if (first) first = false;
        else str += ", ";
        str += name + ": " + type->String;
    }
    return str + " }";
}

NJS::ObjectType::ObjectType(const std::map<std::string, TypePtr>& element_types)
    : Type(GenString(element_types))
{
    for (const auto& [name, type] : element_types)
        ElementTypes.emplace_back(name, type);
}

bool NJS::ObjectType::IsObject() const
{
    return true;
}

NJS::TypePtr NJS::ObjectType::Member(const std::string& name)
{
    for (const auto& [name_, type_] : ElementTypes)
        if (name_ == name) return type_;
    Error("undefined member");
}

size_t NJS::ObjectType::MemberIndex(const std::string& name)
{
    for (size_t i = 0; i < ElementTypes.size(); ++i)
        if (ElementTypes[i].first == name) return i;
    Error("undefined member");
}

NJS::TypePtr NJS::ObjectType::Element(const size_t i)
{
    return ElementTypes[i].second;
}

size_t NJS::ObjectType::NumElements() const
{
    return ElementTypes.size();
}

void NJS::ObjectType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_OBJECT));
    args.push_back(builder.GetBuilder().getInt64(ElementTypes.size()));
    for (const auto& [name, element] : ElementTypes)
    {
        args.push_back(ConstStringExpr::GetString(builder, name));
        element->TypeInfo(builder, args);
    }
}

size_t NJS::ObjectType::Bytes() const
{
    size_t bytes = 0;
    for (const auto& [name, type] : ElementTypes)
        bytes += type->Bytes();
    return bytes;
}

llvm::Type* NJS::ObjectType::GenLLVM(Builder& builder) const
{
    std::vector<llvm::Type*> elements;
    for (const auto& [name, element] : ElementTypes)
        elements.push_back(element->GenLLVM(builder));
    return llvm::StructType::get(builder.GetContext(), elements);
}
