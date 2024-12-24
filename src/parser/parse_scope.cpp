#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::StmtPtr NJS::Parser::ParseScopeStmt()
{
    std::vector<StmtPtr> children;

    const auto where = Expect("{").Where;
    while (!At("}") && !AtEof())
        children.push_back(ParseStmt());
    Expect("}");

    return std::make_shared<ScopeStmt>(where, children);
}

NJS::ExprPtr NJS::Parser::ParseScopeExpr()
{
    std::vector<StmtPtr> children;

    const auto where = Expect("{").Where;
    while (!At("}") && !AtEof())
        children.push_back(ParseStmt());
    Expect("}");

    ExprPtr last = std::dynamic_pointer_cast<Expr>(children.back());
    children.pop_back();

    return std::make_shared<ScopeExpr>(where, children, last);
}
