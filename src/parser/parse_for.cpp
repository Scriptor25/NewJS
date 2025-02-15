#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseForStatement()
{
    const auto where = Expect("for").Where;

    Expect("(");

    StatementPtr init;
    if (!NextAt(";"))
    {
        init = ParseStatement();
        Expect(";");
    }

    ExpressionPtr condition;
    if (!NextAt(";"))
    {
        condition = ParseExpression();
        Expect(";");
    }

    StatementPtr loop;
    if (!NextAt(")"))
    {
        loop = ParseStatement();
        Expect(")");
    }

    const auto body = ParseStatement();

    return std::make_shared<ForStatement>(where, init, condition, loop, body);
}
