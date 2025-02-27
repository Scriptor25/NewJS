#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>

NJS::CastExpression::CastExpression(SourceLocation where, TypePtr type, ExpressionPtr operand)
    : Expression(std::move(where)),
      Type(std::move(type)),
      Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::CastExpression::GenLLVM(Builder &builder, ErrorInfo &error, const TypePtr &) const
{
    auto value = Operand->GenLLVM(builder, error, Type);
    return builder.CreateCast(Where, value, Type);
}

std::ostream &NJS::CastExpression::Print(std::ostream &stream)
{
    return Type->Print(Operand->Print(stream) << " as ");
}
