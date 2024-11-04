#include <ranges>
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
}

NJS::TypePtr& NJS::Context::GetType(const std::string& str)
{
    return m_TypeMap[str];
}

NJS::TypePtr NJS::Context::GetPrimitiveType(TypeName name)
{
    auto& ref = GetType(PrimitiveType::GenString(name));
    if (ref) return ref;
    return ref = std::make_shared<PrimitiveType>(name);
}

NJS::TypePtr NJS::Context::GetVoidType()
{
    return GetPrimitiveType(Type_Void);
}

NJS::TypePtr NJS::Context::GetBooleanType()
{
    return GetPrimitiveType(Type_Boolean);
}

NJS::TypePtr NJS::Context::GetNumberType()
{
    return GetPrimitiveType(Type_Number);
}

NJS::TypePtr NJS::Context::GetStringType()
{
    return GetPrimitiveType(Type_String);
}

NJS::TypePtr NJS::Context::GetTupleType(const std::vector<TypePtr>& element_types)
{
    auto& ref = GetType(TupleType::GenString(element_types));
    if (ref) return ref;
    return ref = std::make_shared<TupleType>( element_types);
}

NJS::TypePtr NJS::Context::GetObjectType(const std::map<std::string, TypePtr>& element_types)
{
    auto& ref = GetType(ObjectType::GenString(element_types));
    if (ref) return ref;
    return ref = std::make_shared<ObjectType>(element_types);
}

NJS::TypePtr NJS::Context::GetArrayType(const TypePtr& element_type)
{
    auto& ref = GetType(ArrayType::GenString(element_type));
    if (ref) return ref;
    return ref = std::make_shared<ArrayType>(element_type);
}

NJS::TypePtr NJS::Context::GetFunctionType(
    const std::vector<TypePtr>& param_types,
    const TypePtr& result_type,
    const bool vararg)
{
    auto& ref = GetType(FunctionType::GenString(param_types, result_type, vararg));
    if (ref) return ref;
    return ref = std::make_shared<FunctionType>(param_types, result_type, vararg);
}
