#include <utility>
#include <NJS/AST.hpp>

NJS::Stmt::Stmt(SourceLocation where)
    : Where(std::move(where))
{
}

NJS::Expr::Expr(SourceLocation where)
    : Stmt(std::move(where))
{
}

void NJS::Expr::GenVoidLLVM(Builder& builder)
{
    (void)GenLLVM(builder, {});
}
