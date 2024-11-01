#include <NJS/Type.hpp>

std::ostream& NJS::operator<<(std::ostream& os, const TypePtr& type)
{
    return type->Print(os);
}

NJS::Type::Type(std::string str)
    : String(std::move(str))
{
}

NJS::TypePtr NJS::Type::Member(const std::string&)
{
    return {};
}

NJS::TypePtr NJS::Type::Result()
{
    return {};
}

NJS::TypePtr NJS::Type::Element()
{
    return {};
}

NJS::TypePtr NJS::Type::Element(size_t)
{
    return {};
}

std::ostream& NJS::Type::Print(std::ostream& os)
{
    return os << String;
}

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

std::string NJS::PrimitiveType::GenString(const TypeName name)
{
    static std::map<TypeName, const char*> type_names
    {
        {Type_Void, "void"},
        {Type_Undefined, "undefined"},
        {Type_Boolean, "boolean"},
        {Type_Number, "number"},
        {Type_String, "string"},
    };
    return type_names[name];
}

NJS::PrimitiveType::PrimitiveType(const TypeName name)
    : Type(GenString(name)), Name(name)
{
}

std::string NJS::ArrayType::GenString(const TypePtr& element_type)
{
    return element_type->String + "[]";
}

NJS::ArrayType::ArrayType(TypePtr element_type)
    : Type(GenString(element_type)), ElementType(std::move(element_type))
{
}

NJS::TypePtr NJS::ArrayType::Element()
{
    return ElementType;
}

NJS::TypePtr NJS::ArrayType::Element(const size_t i)
{
    return ElementType;
}

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

NJS::TupleType::TupleType(TypePtr element_type, std::vector<TypePtr> element_types)
    : Type(GenString(element_types)), ElementType(std::move(element_type)), ElementTypes(std::move(element_types))
{
}

NJS::TypePtr NJS::TupleType::Element()
{
    return ElementType;
}

NJS::TypePtr NJS::TupleType::Element(const size_t i)
{
    return ElementTypes[i];
}

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

NJS::ObjectType::ObjectType(std::map<std::string, TypePtr> element_types)
    : Type(GenString(element_types)), ElementTypes(std::move(element_types))
{
}

NJS::TypePtr NJS::ObjectType::Member(const std::string& name)
{
    return ElementTypes[name];
}

std::string NJS::FunctionType::GenString(const std::vector<TypePtr>& param_types, const TypePtr& result_type)
{
    std::string str = "(";
    bool first = true;
    for (const auto& type : param_types)
    {
        if (first) first = false;
        else str += ", ";
        str += type->String;
    }
    return str + ") => " + result_type->String;
}

NJS::FunctionType::FunctionType(std::vector<TypePtr> param_types, TypePtr result_type)
    : Type(GenString(param_types, result_type)),
      ParamTypes(std::move(param_types)),
      ResultType(std::move(result_type))
{
}

NJS::TypePtr NJS::FunctionType::Result()
{
    return ResultType;
}
