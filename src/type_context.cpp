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

NJS::IntTypePtr NJS::TypeContext::GetIntType(unsigned bits, bool is_signed)
{
    return GetType<IntegerType>(bits, is_signed);
}

NJS::FPTypePtr NJS::TypeContext::GetFPType(unsigned bits)
{
    return GetType<FloatingPointType>(bits);
}

NJS::PtrTypePtr NJS::TypeContext::GetPointerType(const TypePtr &element)
{
    return GetType<PointerType>(element);
}

NJS::RefTypePtr NJS::TypeContext::GetRefType(const TypePtr &element)
{
    return GetType<ReferenceType>(element);
}

NJS::ArrayTypePtr NJS::TypeContext::GetArrayType(const TypePtr &element, unsigned count)
{
    return GetType<ArrayType>(element, count);
}

NJS::StructTypePtr NJS::TypeContext::GetStructType(const std::map<std::string, TypePtr> &elements)
{
    return GetType<StructType>(elements);
}

NJS::TupleTypePtr NJS::TypeContext::GetTupleType(const std::vector<TypePtr> &elements)
{
    return GetType<TupleType>(elements);
}

NJS::FunctionTypePtr NJS::TypeContext::GetFunctionType(
    const TypePtr &result,
    const std::vector<TypePtr> &args,
    bool vararg)
{
    return GetType<FunctionType>(result, args, vararg);
}

NJS::IntTypePtr NJS::TypeContext::GetBoolType()
{
    return GetIntType(1, false);
}

NJS::IntTypePtr NJS::TypeContext::GetCharType()
{
    return GetIntType(8, true);
}

NJS::PtrTypePtr NJS::TypeContext::GetStringType()
{
    return GetPointerType(GetIntType(8, true));
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
