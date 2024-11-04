#include <NJS/NJS.hpp>
#include <NJS/Type.hpp>

std::string NJS::MultiType::GenString(const std::vector<TypePtr>& types)
{
    std::map<TypePtr, bool> type_map;
    for (const auto& type : types)
        type_map[type] = true;

    std::string str;
    bool first = true;
    for (const auto& [type, exists] : type_map)
    {
        if (first) first = false;
        else str += " | ";
        str += type->String;
    }
    return str;
}

NJS::MultiType::MultiType(const std::vector<TypePtr>& types)
    : Type(GenString(types))
{
    std::map<TypePtr, bool> type_map;
    for (const auto& type : types)
        type_map[type] = true;
    for (const auto& [type, exists] : type_map)
        Types.push_back(type);
}

llvm::Type* NJS::MultiType::GenLLVM(Builder& builder)
{
    Error("cannot generate llvm for multi-type");
}
