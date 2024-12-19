#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::StmtPtr NJS::Parser::ParseIfStmt()
{
    const auto where = Expect("if").Where;
    StackPush();
    Expect("(");
    const auto condition = ParseExpr();
    Expect(")");
    const auto then = ParseStmt();
    StmtPtr else_;
    if (NextAt("else"))
        else_ = ParseStmt();
    StackPop();
    return std::make_shared<IfStmt>(where, condition, then, else_);
}
