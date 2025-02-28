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

NJS::ValuePtr NJS::TypeOfExpression::GenLLVM(
    Builder &builder,
    const TypePtr &expected_type) const
{
    const auto operand = Operand->GenLLVM(builder, {});
    const auto type = builder.GetTypeContext().GetStringType();
    const auto value = StringExpression::GetString(builder, operand->GetType()->GetString());
    return RValue::Create(builder, type, value);
}

std::ostream &NJS::TypeOfExpression::Print(std::ostream &stream)
{
    return Operand->Print(stream << "typeof(") << ")";
}
