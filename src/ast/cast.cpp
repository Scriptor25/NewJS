#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>

NJS::CastExpression::CastExpression(SourceLocation where, TypePtr type, ExpressionPtr operand)
    : Expression(std::move(where)),
      Type(std::move(type)),
      Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::CastExpression::GenLLVM(Builder &builder, const TypePtr &) const
{
    const auto value = Operand->GenLLVM(builder, Type);
    return builder.CreateCast(Where, value, Type);
}

std::ostream &NJS::CastExpression::Print(std::ostream &stream)
{
    return Type->Print(Operand->Print(stream) << " as ");
}
