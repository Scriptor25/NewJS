#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::CharExpression::CharExpression(SourceLocation where, const char value)
    : Expression(std::move(where)),
      Value(value)
{
}

NJS::ValuePtr NJS::CharExpression::GenLLVM(Builder &builder, ErrorInfo &error, const TypePtr &) const
{
    const auto result_type = builder.GetTypeContext().GetCharType();
    const auto result_value = builder.GetBuilder().getInt8(Value);
    return RValue::Create(builder, result_type, result_value);
}

std::ostream &NJS::CharExpression::Print(std::ostream &stream)
{
    return stream << '\'' << Value << '\'';
}
