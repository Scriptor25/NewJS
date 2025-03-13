#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::ScopeStatement::ScopeStatement(SourceLocation where, std::vector<StatementPtr> children)
    : Statement(std::move(where)),
      Children(std::move(children))
{
}

void NJS::ScopeStatement::PGenLLVM(Builder &builder)
{
    builder.StackPush();
    for (const auto &child: Children)
        child->GenLLVM(builder);
    builder.StackPop();
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

NJS::ScopeExpression::ScopeExpression(SourceLocation where, std::vector<StatementPtr> children, ExpressionPtr last)
    : Expression(std::move(where)),
      Children(std::move(children)),
      Last(std::move(last))
{
}

NJS::ValuePtr NJS::ScopeExpression::PGenLLVM(Builder &builder, const TypePtr &expected_type)
{
    builder.StackPush();
    for (const auto &child: Children)
        child->GenLLVM(builder);
    auto result = Last->GenLLVM(builder, expected_type);
    builder.StackPop();
    return result;
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
