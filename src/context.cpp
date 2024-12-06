#include <NJS/Context.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Type.hpp>

NJS::Context::Context()
{
    GetVoidType();
    GetBooleanType();
    GetNumberType();
    GetStringType();
    GetCharType();
}

NJS::TypePtr& NJS::Context::GetType(const std::string& str)
{
    return m_TypeMap[str];
}

std::shared_ptr<NJS::PrimitiveType> NJS::Context::GetPrimitiveType(Primitive name)
{
    return GetType<PrimitiveType>(name);
}

std::shared_ptr<NJS::PrimitiveType> NJS::Context::GetVoidType()
{
    return GetPrimitiveType(Primitive_Void);
}

std::shared_ptr<NJS::PrimitiveType> NJS::Context::GetBooleanType()
{
    return GetPrimitiveType(Primitive_Boolean);
}

std::shared_ptr<NJS::PrimitiveType> NJS::Context::GetNumberType()
{
    return GetPrimitiveType(Primitive_Number);
}

std::shared_ptr<NJS::PrimitiveType> NJS::Context::GetStringType()
{
    return GetPrimitiveType(Primitive_String);
}

std::shared_ptr<NJS::PrimitiveType> NJS::Context::GetCharType()
{
    return GetPrimitiveType(Primitive_Char);
}

std::shared_ptr<NJS::TupleType> NJS::Context::GetTupleType(const std::vector<TypePtr>& element_types)
{
    return GetType<TupleType>(element_types);
}

std::shared_ptr<NJS::ObjectType> NJS::Context::GetObjectType(const std::map<std::string, TypePtr>& element_types)
{
    return GetType<ObjectType>(element_types);
}

std::shared_ptr<NJS::ArrayType> NJS::Context::GetArrayType(const TypePtr& element_type, const size_t element_count)
{
    return GetType<ArrayType>(element_type, element_count);
}

std::shared_ptr<NJS::FunctionType> NJS::Context::GetFunctionType(
    const std::vector<TypePtr>& param_types,
    const TypePtr& result_type,
    const bool vararg)
{
    return GetType<FunctionType>(param_types, result_type, vararg);
}

std::shared_ptr<NJS::VectorType> NJS::Context::GetVectorType(const TypePtr& element_type)
{
    return GetType<VectorType>(element_type);
}
