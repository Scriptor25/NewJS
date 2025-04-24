#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::TypeOfExpression::TypeOfExpression(SourceLocation where, ExpressionPtr operand)
    : Expression(std::move(where)),
      Operand(std::move(operand))
{
}

std::ostream &NJS::TypeOfExpression::Print(std::ostream &stream) const
{
    return Operand->Print(stream << "typeof(") << ")";
}

NJS::ValuePtr NJS::TypeOfExpression::_GenIntermediate(Builder &builder, const TypePtr &expected_type)
{
    const auto operand = Operand->GenIntermediate(builder, nullptr);
    const auto type = builder.GetTypeContext().GetStringType();
    const auto value = builder.GetString(operand->GetType()->GetString());
    return RValue::Create(builder, type, value);
}
