#include <NJS/AST.hpp>
#include <NJS/Context.hpp>
#include <NJS/Parser.hpp>

NJS::ScopeStmtPtr NJS::Parser::ParseScope()
{
    std::vector<StmtPtr> children;

    Expect("{");
    while (!NextAt("}"))
        children.push_back(ParseLine());

    return std::make_shared<ScopeStmt>(children);
}
