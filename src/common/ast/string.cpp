#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::StringExpression::StringExpression(SourceLocation where, std::string value)
    : Expression(std::move(where)),
      Value(std::move(value))
{
}

NJS::ValuePtr NJS::StringExpression::PGenLLVM(Builder &builder, const TypePtr &) const
{
    const auto type = builder.GetTypeContext().GetStringType();
    const auto value = builder.GetString(Value);
    return RValue::Create(builder, type, value);
}

std::ostream &NJS::StringExpression::Print(std::ostream &stream) const
{
    return stream << '"' << Value << '"';
}
