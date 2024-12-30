#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>

NJS::CastExpr::CastExpr(SourceLocation where, TypePtr type, ExprPtr value)
    : Expr(std::move(where)), Type(std::move(type)), Value(std::move(value))
{
}

NJS::ValuePtr NJS::CastExpr::GenLLVM(Builder& builder, const TypePtr&) const
{
    const auto value = Value->GenLLVM(builder, Type);
    return builder.CreateCast(Where, value, Type);
}

std::ostream& NJS::CastExpr::Print(std::ostream& os)
{
    return Type->Print(Value->Print(os) << " as ");
}
