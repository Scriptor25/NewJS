#include <NJS/AST.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseVariableStatement(const bool is_extern)
{
    const auto where = m_Token.Where;

    unsigned flags = ParameterFlags_None;
    if (is_extern)
        flags |= ParameterFlags_Extern;

    const auto is_const = NextAt("const");
    if (is_const)
        flags |= ParameterFlags_Const;
    else
        Expect("let");

    const auto parameter = ParseParameter();

    ExpressionPtr value;
    if ((!is_extern && (is_const || parameter->RequireValue()) && (Expect("="), true)) || NextAt("="))
        value = ParseExpression();

    return std::make_shared<VariableStatement>(where, parameter, flags, value);
}
