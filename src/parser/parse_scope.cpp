#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseScopeStatement()
{
    std::vector<StatementPtr> children;

    const auto where = Expect("{").Where;
    while (!At("}") && !AtEof())
        children.emplace_back(ParseStatement());
    Expect("}");

    return std::make_shared<ScopeStatement>(where, children);
}

NJS::ExpressionPtr NJS::Parser::ParseScopeExpression()
{
    std::vector<StatementPtr> children;

    const auto where = Expect("{").Where;
    while (!At("}") && !AtEof())
        children.emplace_back(ParseStatement());
    Expect("}");

    if (children.empty())
        Error(where, "a scope expression must have at least one child expression");

    auto last = std::dynamic_pointer_cast<Expression>(children.back());
    if (!last)
        Error(where, "last child of scope expression must be an expression of some kind");

    children.pop_back();

    return std::make_shared<ScopeExpression>(where, children, last);
}
