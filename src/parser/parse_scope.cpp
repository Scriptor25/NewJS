#include <NJS/AST.hpp>
#include <NJS/Context.hpp>
#include <NJS/Parser.hpp>

NJS::ScopeStmtPtr NJS::Parser::ParseScope()
{
    std::vector<StmtPtr> children;

    Expect("{");
    while (!At("}") && !AtEof())
        children.push_back(ParseLine());
    Expect("}");

    return std::make_shared<ScopeStmt>(children);
}

NJS::ExprPtr NJS::Parser::ParseScopeExpr()
{
    std::vector<StmtPtr> children;

    Expect("{");
    while (!At("}") && !AtEof())
        children.push_back(ParseLine());
    Expect("}");

    ExprPtr last = std::dynamic_pointer_cast<Expr>(children.back());
    children.pop_back();

    return std::make_shared<ScopeExpr>(children, last);
}
