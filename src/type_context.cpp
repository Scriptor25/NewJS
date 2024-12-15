#include <NJS/TypeContext.hpp>

NJS::TypePtr& NJS::TypeContext::GetType(const std::string& string)
{
    return m_TypeMap[string];
}

NJS::TypePtr NJS::TypeContext::GetVoidType()
{
    return GetType<VoidType>();
}

NJS::TypePtr NJS::TypeContext::GetIntType(unsigned bits, bool is_signed)
{
    return GetType<IntType>(bits, is_signed);
}

NJS::TypePtr NJS::TypeContext::GetFPType(unsigned bits)
{
    return GetType<FPType>(bits);
}

NJS::TypePtr NJS::TypeContext::GetPointerType(TypePtr element)
{
    return GetType<PointerType>(element);
}

NJS::TypePtr NJS::TypeContext::GetArrayType(TypePtr element, unsigned count)
{
    return GetType<ArrayType>(element, count);
}

NJS::TypePtr NJS::TypeContext::GetStructType(std::map<std::string, TypePtr> elements)
{
    return GetType<StructType>(elements);
}

NJS::TypePtr NJS::TypeContext::GetTupleType(std::vector<TypePtr> elements)
{
    return GetType<TupleType>(elements);
}

NJS::TypePtr NJS::TypeContext::GetFunctionType(TypePtr result, std::vector<TypePtr> args, bool vararg)
{
    return GetType<FunctionType>(result, args, vararg);
}
