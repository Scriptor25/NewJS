#include <NJS/AST.hpp>

NJS::SubscriptExpr::SubscriptExpr(ExprPtr array, ExprPtr index)
    : Array(std::move(array)), Index(std::move(index))
{
}

NJS::ValuePtr NJS::SubscriptExpr::GenLLVM(Builder& builder)
{
}

std::ostream& NJS::SubscriptExpr::Print(std::ostream& os)
{
    return Index->Print(Array->Print(os) << '[') << ']';
}
