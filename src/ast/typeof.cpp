#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::TypeOfExpression::TypeOfExpression(SourceLocation where, ExpressionPtr operand)
    : Expression(std::move(where)),
      Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::TypeOfExpression::GenLLVM(Builder &builder, const TypePtr &expected) const
{
    const auto operand = Operand->GenLLVM(builder, {});
    const auto type = builder.GetTypeContext().GetStringType();
    const auto value = StringExpression::GetString(builder, operand->GetType()->GetString());
    return RValue::Create(builder, type, value);
}

std::ostream &NJS::TypeOfExpression::Print(std::ostream &os)
{
    return Operand->Print(os << "typeof(") << ")";
}
