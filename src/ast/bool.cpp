#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::BooleanExpression::BooleanExpression(SourceLocation where, const bool value)
    : Expression(std::move(where)),
      Value(value)
{
}

NJS::ValuePtr NJS::BooleanExpression::GenLLVM(Builder &builder, const TypePtr &) const
{
    const auto result_type = builder.GetTypeContext().GetBooleanType();
    const auto result_value = builder.GetBuilder().getInt1(Value);
    return RValue::Create(builder, result_type, result_value);
}

std::ostream &NJS::BooleanExpression::Print(std::ostream &stream)
{
    return stream << (Value ? "true" : "false");
}
