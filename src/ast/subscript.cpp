#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>

NJS::SubscriptExpr::SubscriptExpr(SourceLocation where, ExprPtr array, ExprPtr index)
    : Expr(std::move(where)), Array(std::move(array)), Index(std::move(index))
{
}

NJS::ValuePtr NJS::SubscriptExpr::GenLLVM(Builder& builder)
{
    const auto array = Array->GenLLVM(builder);
    const auto index = Index->GenLLVM(builder);

    return builder.CreateSubscript(array, index);
}

std::ostream& NJS::SubscriptExpr::Print(std::ostream& os)
{
    return Index->Print(Array->Print(os) << '[') << ']';
}
