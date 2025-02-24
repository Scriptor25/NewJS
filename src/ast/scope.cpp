#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>

NJS::ScopeStatement::ScopeStatement(SourceLocation where, std::vector<StatementPtr> children)
    : Statement(std::move(where)),
      Children(std::move(children))
{
}

void NJS::ScopeStatement::GenVoidLLVM(Builder &builder) const
{
    builder.StackPush();
    for (const auto &child: Children)
        child->GenVoidLLVM(builder);
    builder.StackPop();
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

NJS::ValuePtr NJS::ScopeExpression::GenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    builder.StackPush();
    for (const auto &child: Children)
        child->GenVoidLLVM(builder);
    auto result = Last->GenLLVM(builder, expected_type);
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
