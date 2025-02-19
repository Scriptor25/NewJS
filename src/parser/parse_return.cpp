#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseReturnStatement()
{
    const auto where = Expect("return").Where;

    ExpressionPtr value;
    if (!NextAt("void"))
        value = ParseExpression();

    return std::make_shared<ReturnStatement>(where, value);
}
