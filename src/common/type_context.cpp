#include <newjs/error.hpp>
#include <newjs/parameter.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>

const NJS::TypePtr &NJS::TypeContext::GetType(const std::string &string) const
{
    if (!m_TemplateStack.empty())
    {
        if (m_TemplateStack.back().contains(string))
            return m_TemplateStack.back().at(string);
        Error("no type {}", string);
    }
    if (m_Types.contains(string))
        return m_Types.at(string);
    Error("no type {}", string);
}

NJS::TypePtr &NJS::TypeContext::DefType(const std::string &string)
{
    if (!m_TemplateStack.empty())
        return m_TemplateStack.back()[string];
    return m_Types[string];
}

NJS::IncompleteTypePtr NJS::TypeContext::GetIncompleteType(const std::string &name)
{
    return GetType<IncompleteType>(name);
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

NJS::StructTypePtr NJS::TypeContext::GetUnsafeStructType(
    const std::vector<std::pair<std::string, TypePtr>> &element_types)
{
    std::vector<std::pair<std::string, ReferenceInfo>> elements(element_types.size());
    for (unsigned i = 0; i < element_types.size(); ++i)
        elements[i] = {element_types[i].first, ReferenceInfo(element_types[i].second)};
    return GetStructType(elements);
}

NJS::StructTypePtr NJS::TypeContext::GetStructType(const std::vector<std::pair<std::string, ReferenceInfo>> &elements)
{
    return GetType<StructType>(elements);
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

void NJS::TypeContext::PushTemplate(const std::vector<std::string> &names, const std::vector<TypePtr> &types)
{
    auto copy = m_TemplateStack.empty() ? m_Types : m_TemplateStack.back();
    auto &map = m_TemplateStack.emplace_back(copy);
    for (unsigned i = 0; i < names.size(); ++i)
        map[names[i]] = types[i];
}

void NJS::TypeContext::PopTemplate()
{
    m_TemplateStack.pop_back();
}
