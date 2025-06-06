#include <newjs/ast.hpp>
#include <newjs/parser.hpp>

NJS::StatementPtr NJS::Parser::ParseWhileStatement()
{
    const auto where = Expect("while").Where;
    Expect("(");
    const auto condition = ParseExpression();
    Expect(")");
    const auto body = ParseStatement();
    return std::make_shared<WhileStatement>(where, condition, body);
}
