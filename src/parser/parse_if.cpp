#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::IfStmtPtr NJS::Parser::ParseIf()
{
    Expect("if");
    Expect("(");
    const auto condition = ParseExpression();
    Expect(")");
    const auto then = ParseLine();
    StmtPtr else_;
    if (NextAt("else"))
        else_ = ParseLine();
    return std::make_shared<IfStmt>(condition, then, else_);
}
