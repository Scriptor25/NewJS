#include <NJS/TypeContext.hpp>

NJS::TypePtr& NJS::TypeContext::GetType(const std::string& string)
{
    return m_TypeMap[string];
}

NJS::NoTypePtr NJS::TypeContext::GetNoType()
{
    return GetType<NoType>();
}

NJS::VoidTypePtr NJS::TypeContext::GetVoidType()
{
    return GetType<VoidType>();
}

NJS::IntTypePtr NJS::TypeContext::GetIntType(unsigned bits, bool is_signed)
{
    return GetType<IntType>(bits, is_signed);
}

NJS::FPTypePtr NJS::TypeContext::GetFPType(unsigned bits)
{
    return GetType<FPType>(bits);
}

NJS::PtrTypePtr NJS::TypeContext::GetPointerType(const TypePtr& element)
{
    return GetType<PtrType>(element);
}

NJS::RefTypePtr NJS::TypeContext::GetRefType(TypePtr element)
{
    return GetType<RefType>(element);
}

NJS::ArrayTypePtr NJS::TypeContext::GetArrayType(const TypePtr& element, unsigned count)
{
    return GetType<ArrayType>(element, count);
}

NJS::StructTypePtr NJS::TypeContext::GetStructType(const std::map<std::string, TypePtr>& elements)
{
    return GetType<StructType>(elements);
}

NJS::TupleTypePtr NJS::TypeContext::GetTupleType(const std::vector<TypePtr>& elements)
{
    return GetType<TupleType>(elements);
}

NJS::FunctionTypePtr NJS::TypeContext::GetFunctionType(
    const TypePtr& result,
    const std::vector<TypePtr>& args,
    bool vararg)
{
    return GetType<FunctionType>(result, args, vararg);
}

NJS::IntTypePtr NJS::TypeContext::GetBoolType()
{
    return GetIntType(1, false);
}

NJS::PtrTypePtr NJS::TypeContext::GetStringType()
{
    return GetPointerType(GetIntType(8, true));
}
