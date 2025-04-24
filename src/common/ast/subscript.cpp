#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::SubscriptExpression::SubscriptExpression(SourceLocation where, ExpressionPtr array, ExpressionPtr index)
    : Expression(std::move(where)),
      Array(std::move(array)),
      Index(std::move(index))
{
}

std::ostream &NJS::SubscriptExpression::Print(std::ostream &stream) const
{
    return Index->Print(Array->Print(stream) << '[') << ']';
}

NJS::ValuePtr NJS::SubscriptExpression::_GenIntermediate(Builder &builder, const TypePtr &)
{
    const auto array = Array->GenIntermediate(builder, nullptr);
    const auto index = Index->GenIntermediate(builder, nullptr);

    return builder.CreateSubscript(array, index);
}
