#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::BooleanExpression::BooleanExpression(SourceLocation where, const bool value)
    : Expression(std::move(where)),
      Value(value)
{
}

std::ostream &NJS::BooleanExpression::Print(std::ostream &stream) const
{
    return stream << (Value ? "true" : "false");
}

NJS::ValuePtr NJS::BooleanExpression::_GenIntermediate(Builder &builder, const TypePtr &)
{
    const auto result_type = builder.GetTypeContext().GetBooleanType();
    const auto result_value = builder.GetBuilder().getInt1(Value);
    return RValue::Create(builder, result_type, result_value);
}
