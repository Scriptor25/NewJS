#include <NJS/AST.hpp>
#include <NJS/Context.hpp>
#include <NJS/Parser.hpp>

NJS::ScopeStmtPtr NJS::Parser::ParseScope()
{
    std::vector<StmtPtr> children;

    m_Ctx.StackPush();
    Expect("{");
    while (!NextAt("}"))
        children.push_back(ParseLine());
    m_Ctx.StackPop();

    return std::make_shared<ScopeStmt>(children);
}
