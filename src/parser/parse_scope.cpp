#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::StatementPtr NJS::Parser::ParseScopeStatement()
{
    std::vector<StatementPtr> children;

    const auto where = Expect("{").Where;
    while (!At("}") && !AtEof())
        children.push_back(ParseStatement());
    Expect("}");

    return std::make_shared<ScopeStatement>(where, children);
}

NJS::ExpressionPtr NJS::Parser::ParseScopeExpression()
{
    std::vector<StatementPtr> children;

    const auto where = Expect("{").Where;
    while (!At("}") && !AtEof())
        children.push_back(ParseStatement());
    Expect("}");

    auto last = std::dynamic_pointer_cast<Expression>(children.back());
    children.pop_back();

    return std::make_shared<ScopeExpression>(where, children, last);
}
