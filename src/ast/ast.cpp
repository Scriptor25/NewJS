#include <utility>
#include <NJS/AST.hpp>

NJS::Stmt::Stmt(SourceLocation where)
    : Where(std::move(where))
{
}

NJS::Expr::Expr(SourceLocation where, TypePtr type)
    : Stmt(std::move(where)), Type(std::move(type))
{
}
