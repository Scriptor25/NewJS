#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::ScopeStatement::ScopeStatement(SourceLocation where, std::vector<StatementPtr> children)
    : Statement(std::move(where)),
      Children(std::move(children))
{
}

bool NJS::ScopeStatement::GenLLVM(Builder &builder) const
{
    builder.StackPush();
    for (const auto &child: Children)
        if (child->GenLLVM(builder))
            return true;
    builder.StackPop();
    return false;
}

std::ostream &NJS::ScopeStatement::Print(std::ostream &stream)
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

NJS::ValuePtr NJS::ScopeExpression::GenLLVM(
    Builder &builder,
    const TypePtr &expected_type) const
{
    builder.StackPush();
    for (const auto &child: Children)
        if (child->GenLLVM(builder))
            return nullptr;
    auto result = Last->GenLLVM(builder, expected_type);
    if (!result)
        return nullptr;
    builder.StackPop();
    return result;
}

std::ostream &NJS::ScopeExpression::Print(std::ostream &stream)
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
