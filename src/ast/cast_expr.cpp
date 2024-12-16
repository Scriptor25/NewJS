#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>

NJS::CastExpr::CastExpr(SourceLocation where, TypePtr type, ExprPtr value)
    : Expr(std::move(where), std::move(type)), Value(std::move(value))
{
}

NJS::ValuePtr NJS::CastExpr::GenLLVM(Builder& builder)
{
    Error("TODO");
}

std::ostream& NJS::CastExpr::Print(std::ostream& os)
{
    return Type->Print(Value->Print(os) << " as ");
}
