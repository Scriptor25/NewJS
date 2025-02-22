#include <NJS/AST.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseVariableStatement(const bool is_extern)
{
    const auto where = m_Token.Where;

    const auto is_const = NextAt("const");
    if (!is_const)
        Expect("let");

    auto is_reference = NextAt("&");

    const auto parameter = ParseParameter(is_const, is_reference);

    ExpressionPtr value;
    if ((!is_extern && (is_const || is_reference || parameter->RequireValue()) && (Expect("="), true)) || NextAt("="))
        value = ParseExpression();

    return std::make_shared<VariableStatement>(where, is_extern, is_const, is_reference, parameter, value);
}
