#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>

NJS::StmtPtr NJS::Parser::ParseForStmt()
{
    StmtPtr init, loop;
    ExprPtr condition;

    const auto where = Expect("for").Where;

    Expect("(");
    StackPush();
    if (!NextAt(";"))
    {
        init = ParseStmt();
        Expect(";");
    }
    if (!NextAt(";"))
    {
        condition = ParseExpr();
        Expect(";");
    }
    if (!NextAt(")"))
    {
        loop = ParseStmt();
        Expect(")");
    }

    const auto body = ParseStmt();

    StackPop();
    return std::make_shared<ForStmt>(where, init, condition, loop, body);
}
