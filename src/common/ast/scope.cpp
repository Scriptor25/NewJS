#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::ScopeStatement::ScopeStatement(SourceLocation where, std::vector<StatementPtr> children)
    : Statement(std::move(where)),
      Children(std::move(children))
{
}

std::ostream &NJS::ScopeStatement::Print(std::ostream &stream) const
{
    if (Children.empty())
        return stream << "{}";

    stream << '{' << std::endl;
    Indent();
    for (const auto &child: Children)
        child->Print(Spacing(stream)) << std::endl;
    Exdent();
    return Spacing(stream) << '}';
}

void NJS::ScopeStatement::_GenIntermediate(Builder &builder, bool)
{
    builder.StackPush();
    for (const auto &child: Children)
        child->GenIntermediate(builder, false);
    builder.StackPop();
}

NJS::ScopeExpression::ScopeExpression(SourceLocation where, std::vector<StatementPtr> children, ExpressionPtr last)
    : Expression(std::move(where)),
      Children(std::move(children)),
      Last(std::move(last))
{
}

std::ostream &NJS::ScopeExpression::Print(std::ostream &stream) const
{
    if (Children.empty())
        return Last->Print(stream << "{ ") << " }";

    stream << '{' << std::endl;
    Indent();
    for (const auto &child: Children)
        child->Print(Spacing(stream)) << std::endl;
    Last->Print(Spacing(stream)) << std::endl;
    Exdent();
    return Spacing(stream) << '}';
}

NJS::ValuePtr NJS::ScopeExpression::_GenIntermediate(Builder &builder, const TypePtr &expected_type)
{
    builder.StackPush();
    for (const auto &child: Children)
        child->GenIntermediate(builder, false);
    auto result = Last->GenIntermediate(builder, expected_type);
    builder.StackPop();
    return result;
}
