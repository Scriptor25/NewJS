#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::Builder& NJS::Value::GetBuilder() const
{
    return m_Builder;
}

NJS::TypePtr NJS::Value::GetType() const
{
    return m_Type;
}

NJS::Value::Value(Builder& builder, TypePtr type)
    : m_Builder(builder), m_Type(std::move(type))
{
}
