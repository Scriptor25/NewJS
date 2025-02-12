#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseIfStatement()
{
    const auto where = Expect("if").Where;

    Expect("(");

    const auto condition = ParseExpression();

    Expect(")");

    const auto then = ParseStatement();

    StatementPtr else_;
    if (NextAt("else"))
        else_ = ParseStatement();

    return std::make_shared<IfStatement>(where, condition, then, else_);
}
