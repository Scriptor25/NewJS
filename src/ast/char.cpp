#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::CharacterExpression::CharacterExpression(SourceLocation where, const char value)
    : Expression(std::move(where)),
      Value(value)
{
}

NJS::ValuePtr NJS::CharacterExpression::GenLLVM(Builder &builder, const TypePtr &) const
{
    const auto result_type = builder.GetTypeContext().GetCharType();
    const auto result_value = builder.GetBuilder().getInt8(Value);
    return RValue::Create(builder, result_type, result_value);
}

std::ostream &NJS::CharacterExpression::Print(std::ostream &os)
{
    return os << '\'' << Value << '\'';
}
