#include <NJS/AST.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseVariableStatement()
{
    const auto where = m_Token.Where;
    const auto is_const = NextAt("const");
    if (!is_const)
        Expect("let");

    const auto name = ParseParam();

    ExpressionPtr value;
    if (((is_const || name->RequireValue()) && (Expect("="), true)) || NextAt("="))
        value = ParseExpression();

    return std::make_shared<VariableStatement>(where, is_const, name, value);
}
