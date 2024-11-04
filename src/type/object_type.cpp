#include <llvm/IR/DerivedTypes.h>
#include <NJS/Builder.hpp>
#include <NJS/NJS.hpp>
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

size_t NJS::ObjectType::Size()
{
    size_t size = 0;
    for (const auto& [name, type] : ElementTypes)
        size += type->Size();
    return size;
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

llvm::Type* NJS::ObjectType::GenLLVM(Builder& builder)
{
    std::vector<llvm::Type*> elements(ElementTypes.size());
    for (size_t i = 0; i < ElementTypes.size(); ++i)
        elements[i] = ElementTypes[i].second->GenLLVM(builder);
    return llvm::StructType::get(builder.LLVMContext(), elements);
}
