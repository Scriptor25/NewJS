#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>

NJS::SubscriptExpr::SubscriptExpr(SourceLocation where, TypePtr type, ExprPtr array, ExprPtr index)
    : Expr(std::move(where), std::move(type)), Array(std::move(array)), Index(std::move(index))
{
}

NJS::ValuePtr NJS::SubscriptExpr::GenLLVM(Builder& builder)
{
    Error("TODO");
}

std::ostream& NJS::SubscriptExpr::Print(std::ostream& os)
{
    return Index->Print(Array->Print(os) << '[') << ']';
}
