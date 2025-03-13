#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>

NJS::CastExpression::CastExpression(SourceLocation where, TypePtr type, ExpressionPtr operand)
    : Expression(std::move(where)),
      Type(std::move(type)),
      Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::CastExpression::PGenLLVM(Builder &builder, const TypePtr &)
{
    return builder.CreateCast(Operand->GenLLVM(builder, Type), Type);
}

std::ostream &NJS::CastExpression::Print(std::ostream &stream) const
{
    return Type->Print(Operand->Print(stream) << " as ");
}
