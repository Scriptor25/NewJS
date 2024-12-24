#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>

NJS::StmtPtr NJS::Parser::ParseForStmt()
{
    const auto where = Expect("for").Where;

    Expect("(");

    StmtPtr init;
    if (!NextAt(";"))
    {
        init = ParseStmt();
        Expect(";");
    }

    ExprPtr condition;
    if (!NextAt(";"))
    {
        condition = ParseExpr();
        Expect(";");
    }

    StmtPtr loop;
    if (!NextAt(")"))
    {
        loop = ParseStmt();
        Expect(")");
    }

    const auto body = ParseStmt();

    return std::make_shared<ForStmt>(where, init, condition, loop, body);
}
