#include <NJS/AST.hpp>

NJS::SubscriptExpr::SubscriptExpr(TypePtr type, ExprPtr array, ExprPtr index)
    : Expr(std::move(type)), Array(std::move(array)), Index(std::move(index))
{
}

NJS::ValuePtr NJS::SubscriptExpr::GenLLVM(Builder& builder)
{
}

std::ostream& NJS::SubscriptExpr::Print(std::ostream& os)
{
    return os << Array << '[' << Index << ']';
}
