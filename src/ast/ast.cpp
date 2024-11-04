#include <NJS/AST.hpp>

std::ostream& NJS::operator<<(std::ostream& os, const StmtPtr& ptr)
{
    return ptr->Print(os);
}

std::ostream& NJS::operator<<(std::ostream& os, const ExprPtr& ptr)
{
    return ptr->Print(os);
}

NJS::Expr::Expr(TypePtr type)
    : Type(std::move(type))
{
}
