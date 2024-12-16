#include <NJS/TypeContext.hpp>

NJS::TypePtr& NJS::TypeContext::GetType(const std::string& string)
{
    return m_TypeMap[string];
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

NJS::PointerTypePtr NJS::TypeContext::GetPointerType(TypePtr element)
{
    return GetType<PointerType>(element);
}

NJS::ArrayTypePtr NJS::TypeContext::GetArrayType(TypePtr element, unsigned count)
{
    return GetType<ArrayType>(element, count);
}

NJS::StructTypePtr NJS::TypeContext::GetStructType(std::map<std::string, TypePtr> elements)
{
    return GetType<StructType>(elements);
}

NJS::TupleTypePtr NJS::TypeContext::GetTupleType(std::vector<TypePtr> elements)
{
    return GetType<TupleType>(elements);
}

NJS::FunctionTypePtr NJS::TypeContext::GetFunctionType(TypePtr result, std::vector<TypePtr> args, bool vararg)
{
    return GetType<FunctionType>(result, args, vararg);
}
