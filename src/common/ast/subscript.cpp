#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::SubscriptExpression::SubscriptExpression(SourceLocation where, ExpressionPtr array, ExpressionPtr index)
    : Expression(std::move(where)),
      Array(std::move(array)),
      Index(std::move(index))
{
}

NJS::ValuePtr NJS::SubscriptExpression::PGenLLVM(Builder &builder, const TypePtr &) const
{
    const auto array = Array->GenLLVM(builder, {});
    const auto index = Index->GenLLVM(builder, {});

    return builder.CreateSubscript(array, index);
}

std::ostream &NJS::SubscriptExpression::Print(std::ostream &stream) const
{
    return Index->Print(Array->Print(stream) << '[') << ']';
}
