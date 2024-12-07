#include <NJS/TypeContext.hpp>

NJS::TypeContext::TypeContext()
{
    GetVoidType();
    GetBooleanType();
    GetNumberType();
    GetStringType();
    GetCharType();
}

NJS::TypePtr& NJS::TypeContext::GetType(const std::string& str)
{
    return m_TypeMap[str];
}

NJS::NoTypePtr NJS::TypeContext::GetNoType()
{
    return GetType<NoType>();
}

NJS::PrimitiveTypePtr NJS::TypeContext::GetPrimitiveType(Primitive name)
{
    return GetType<PrimitiveType>(name);
}

NJS::PrimitiveTypePtr NJS::TypeContext::GetVoidType()
{
    return GetPrimitiveType(Primitive_Void);
}

NJS::PrimitiveTypePtr NJS::TypeContext::GetBooleanType()
{
    return GetPrimitiveType(Primitive_Boolean);
}

NJS::PrimitiveTypePtr NJS::TypeContext::GetNumberType()
{
    return GetPrimitiveType(Primitive_Number);
}

NJS::PrimitiveTypePtr NJS::TypeContext::GetStringType()
{
    return GetPrimitiveType(Primitive_String);
}

NJS::PrimitiveTypePtr NJS::TypeContext::GetCharType()
{
    return GetPrimitiveType(Primitive_Char);
}

NJS::TupleTypePtr NJS::TypeContext::GetTupleType(const std::vector<TypePtr>& element_types)
{
    return GetType<TupleType>(element_types);
}

NJS::ObjectTypePtr NJS::TypeContext::GetObjectType(const std::map<std::string, TypePtr>& element_types)
{
    return GetType<ObjectType>(element_types);
}

NJS::ArrayTypePtr NJS::TypeContext::GetArrayType(const TypePtr& element_type, const size_t element_count)
{
    return GetType<ArrayType>(element_type, element_count);
}

NJS::FunctionTypePtr NJS::TypeContext::GetFunctionType(
    const std::vector<TypePtr>& param_types,
    const TypePtr& result_type,
    const bool vararg)
{
    return GetType<FunctionType>(param_types, result_type, vararg);
}

NJS::VectorTypePtr NJS::TypeContext::GetVectorType(const TypePtr& element_type)
{
    return GetType<VectorType>(element_type);
}
