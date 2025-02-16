#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

NJS::TypePtr &NJS::TypeContext::GetType(const std::string &string)
{
    if (!m_TemplateStack.empty())
        return m_TemplateStack.back()[string];
    return m_Types[string];
}

NJS::NoTypePtr NJS::TypeContext::GetNoType(const std::string &name)
{
    return GetType<NoType>(name);
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

NJS::PointerTypePtr NJS::TypeContext::GetPointerType(const TypePtr &element_type)
{
    return GetType<PointerType>(element_type);
}

NJS::ReferenceTypePtr NJS::TypeContext::GetReferenceType(const TypePtr &element_type)
{
    return GetType<ReferenceType>(element_type);
}

NJS::ArrayTypePtr NJS::TypeContext::GetArrayType(const TypePtr &element_type, unsigned count)
{
    return GetType<ArrayType>(element_type, count);
}

NJS::StructTypePtr NJS::TypeContext::GetStructType(const std::map<std::string, TypePtr> &element_type_map)
{
    return GetType<StructType>(element_type_map);
}

NJS::TupleTypePtr NJS::TypeContext::GetTupleType(const std::vector<TypePtr> &element_types)
{
    return GetType<TupleType>(element_types);
}

NJS::FunctionTypePtr NJS::TypeContext::GetFunctionType(
    const TypePtr &result_type,
    const std::vector<TypePtr> &argument_types,
    bool var_arg)
{
    return GetType<FunctionType>(result_type, argument_types, var_arg);
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
    return GetPointerType(GetIntegerType(8, true));
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
