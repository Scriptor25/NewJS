#include <newjs/ast.hpp>
#include <newjs/parameter.hpp>
#include <newjs/parser.hpp>

NJS::StatementPtr NJS::Parser::ParseVariableStatement(const bool is_extern)
{
    auto where = CurrentLocation();

    const auto is_const = NextAt("const");
    if (!is_const)
        Expect("let");
    const auto is_reference = NextAt("&");

    const auto parameter = ParseParameter(is_const, is_reference);

    ExpressionPtr value;
    if ((parameter->RequireValue() && (Expect("="), true)) || NextAt("="))
        value = ParseExpression();

    return std::make_shared<VariableStatement>(where, is_extern, parameter, value);
}
