#include <newjs/error.hpp>
#include <newjs/parameter.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>

bool NJS::TypeContext::HasNamedType(const std::string &string) const
{
    return m_NamedTypes.contains(string);
}

NJS::TypePtr NJS::TypeContext::GetNamedType(const std::string &string) const
{
    if (m_NamedTypes.contains(string))
        return m_NamedTypes.at(string);
    Error("no type '{}'", string);
}

NJS::TypePtr &NJS::TypeContext::GetNamedTypeReference(const std::string &string)
{
    return m_NamedTypes[string];
}

NJS::TypePtr NJS::TypeContext::GetType(const unsigned hash) const
{
    if (m_Types.contains(hash))
        return m_Types.at(hash);
    Error("no type with hash {}", hash);
}

NJS::TypePtr &NJS::TypeContext::GetTypeReference(const unsigned hash)
{
    return m_Types[hash];
}

NJS::VoidTypePtr NJS::TypeContext::GetVoidType()
{
    return GetType<VoidType>();
}

NJS::IntegerTypePtr NJS::TypeContext::GetIntegerType(unsigned bits, bool is_signed)
{
    return GetType<IntegerType>(bits, is_signed);
}

NJS::FloatingPointTypePtr NJS::TypeContext::GetFloatingPointType(unsigned bits)
{
    return GetType<FloatingPointType>(bits);
}

NJS::PointerTypePtr NJS::TypeContext::GetPointerType(const TypePtr &element_type, bool is_const)
{
    return GetType<PointerType>(element_type, is_const);
}

NJS::ArrayTypePtr NJS::TypeContext::GetArrayType(const TypePtr &element_type, unsigned count)
{
    return GetType<ArrayType>(element_type, count);
}

NJS::StructTypePtr NJS::TypeContext::GetStructType(const std::string &name)
{
    return GetStructType(std::vector<StructElement>(), name);
}

NJS::StructTypePtr NJS::TypeContext::GetStructType(
    const std::vector<std::pair<std::string, TypePtr>> &element_types,
    const std::string &name)
{
    std::vector<StructElement> elements(element_types.size());
    for (unsigned i = 0; i < element_types.size(); ++i)
        elements[i] = {element_types[i].first, ReferenceInfo(element_types[i].second), nullptr};
    return GetStructType(elements, name);
}

NJS::StructTypePtr NJS::TypeContext::GetStructType(const std::vector<StructElement> &elements, std::string name)
{
    if (name.empty())
        name = "S" + std::to_string(StructType::GenHash(elements, {}));
    auto type = GetType<StructType>(elements, name);
    if (type->GetElementCount() == 0 && !elements.empty())
        type->SetElements(elements);
    return type;
}

NJS::TupleTypePtr NJS::TypeContext::GetTupleType(const std::vector<TypePtr> &element_types)
{
    return GetType<TupleType>(element_types);
}

NJS::FunctionTypePtr NJS::TypeContext::GetFunctionType(
    const ReferenceInfo &result,
    const std::vector<ReferenceInfo> &parameters,
    const bool is_var_arg)
{
    return GetType<FunctionType>(result, parameters, is_var_arg);
}

NJS::FunctionTypePtr NJS::TypeContext::GetFunctionType(
    const ReferenceInfo &result,
    const std::vector<ParameterPtr> &parameters,
    const bool is_var_arg)
{
    std::vector<ReferenceInfo> parameter_infos(parameters.size());
    for (unsigned i = 0; i < parameters.size(); ++i)
        parameter_infos[i] = parameters[i]->Info;
    return GetFunctionType(result, parameter_infos, is_var_arg);
}

NJS::IntegerTypePtr NJS::TypeContext::GetBooleanType()
{
    return GetIntegerType(1, false);
}

NJS::IntegerTypePtr NJS::TypeContext::GetCharType()
{
    return GetIntegerType(8, true);
}

NJS::PointerTypePtr NJS::TypeContext::GetStringType()
{
    return GetPointerType(GetIntegerType(8, true), true);
}
