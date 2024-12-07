#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ScopeStmtPtr NJS::Parser::ParseScope()
{
    std::vector<StmtPtr> children;

    StackPush();
    const auto where = Expect("{").Where;
    while (!At("}") && !AtEof())
        children.push_back(ParseLine());
    Expect("}");
    StackPop();

    return std::make_shared<ScopeStmt>(where, children);
}

NJS::ExprPtr NJS::Parser::ParseScopeExpr()
{
    std::vector<StmtPtr> children;

    StackPush();
    const auto where = Expect("{").Where;
    while (!At("}") && !AtEof())
        children.push_back(ParseLine());
    Expect("}");
    StackPop();

    ExprPtr last = std::dynamic_pointer_cast<Expr>(children.back());
    children.pop_back();

    const auto type = last->Type;
    return std::make_shared<ScopeExpr>(where, type, children, last);
}
